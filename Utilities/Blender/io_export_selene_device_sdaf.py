# Copyright (c) 2012 Nezametdinov Ildus
# Licensed under the MIT License (see LICENSE.txt for details)

bl_info = {
    "name": "SELENE Device animation format (.sdaf)",
    "author": "Nezametdinov E. Ildus (everard)",
    "version": (1, 0, 0),
    "blender": (2, 6, 8),
    "location": "File > Export > SELENE Device (.sdaf)",
    "description": "Exports animation to the SELENE Device animation format (.sdaf)",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"}

import bpy
from math import radians
from bpy_extras.io_utils import ExportHelper
from collections import OrderedDict
from mathutils import *
import subprocess
import struct

# Represents bone.
class Bone:
    def __init__(self, armature, rawBone, activeBoneNames):
        self.name = rawBone.name
        self.armature = armature
        self.rawBone = rawBone
        self.activeBoneNames = activeBoneNames

    # Writes bone to the file.
    def write(self, file):
        poseMatrix = self.armature.matrix_world * self.rawBone.matrix
        parentMatrixInv = Matrix.Identity(4)
        if self.rawBone.parent and self.rawBone.parent.name in self.activeBoneNames:
            parentMatrixInv = (self.armature.matrix_world * self.rawBone.parent.matrix).inverted()
        poseMatrix = parentMatrixInv * poseMatrix

        rotation = poseMatrix.to_quaternion()
        position = poseMatrix.to_translation()

        packedBone = struct.pack('<H' + str(len(self.name)) + 'sfffffff',
                                 len(self.name), self.name.encode('ascii', 'ignore'),
                                 rotation.x, rotation.z, rotation.y, -rotation.w,
                                 position.x, position.z, position.y)
        file.write(packedBone)

# Prepares armature.
def prepareArmature(object, armature):
    originalBones = armature.pose.bones
    activeBoneNames = {bone.name for bone in originalBones}.intersection(
                      {group.name for group in object.vertex_groups})
    armatureBones = {bone for bone in originalBones if bone.name in activeBoneNames}
    return {Bone(armature, bone, activeBoneNames) for bone in armatureBones}

# Exports animation
def exportAnimation(object, armature, fileName):
    # write file
    file = open(fileName, 'wb')

    scene = bpy.context.scene
    currentFrame = scene.frame_current

    numFrames = scene.frame_end - scene.frame_start + 1
    if numFrames <= 0:
        return 'FAILURE'

    # write header
    file.write(struct.pack('<4sL', b'SDAF', numFrames))

    # write bones
    bones = prepareArmature(object, armature)

    for frame in range(scene.frame_start, scene.frame_end + 1):
        scene.frame_set(frame)
        file.write(struct.pack('<H', len(bones)))
        for bone in bones:
            bone.write(file)

    scene.frame_set(currentFrame)

    file.flush()
    file.close()
    return 'SUCCESS'

###### EXPORT OPERATOR #######
class ExportSDAF(bpy.types.Operator, ExportHelper):
    bl_idname = "animation.sdaf"
    bl_label = "Export SDAF"

    filename_ext = ".sdaf"

    @classmethod
    def poll(cls, context):
        return context.active_object.type in {'MESH'}

    def execute(self, context):
        print("===============================================================================")
        print('exporting animation...')

        fileName = self.filepath
        fileName = bpy.path.ensure_ext(fileName, self.filename_ext)

        object = bpy.context.active_object

        if object.type != "MESH":
            print("error: could not export animation of the non-mesh object")
            return {'FINISHED'};

        modifiers = [modifier for modifier in object.modifiers\
                     if modifier.type == "ARMATURE" and modifier.show_viewport]

        if len(modifiers) > 1:
            print("error: selected mesh has more than one armature modifier")
            return {'FINISHED'};
        elif len(modifiers) == 0:
            print("error: selected mesh has no armature modifiers")
            return {'FINISHED'};

        modifier = modifiers[0]
        armature = modifier.object
        modifier.show_viewport = False

        print(exportAnimation(object, armature, fileName))
        modifier.show_viewport = True
        return {'FINISHED'};

### REGISTER ###
def menu_func(self, context):
    self.layout.operator(ExportSDAF.bl_idname, text = "SELENE Device animation format (.sdaf)")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
