# Copyright (c) 2012 Nezametdinov Ildus
# Licensed under the MIT License (see LICENSE.txt for details)

bl_info = {
    "name": "SELENE Device mesh format (.sdmf)",
    "author": "Nezametdinov E. Ildus (everard)",
    "version": (1, 0, 0),
    "blender": (2, 6, 8),
    "location": "File > Export > SELENE Device (.sdmf)",
    "description": "Exports mesh to the SELENE Device mesh format (.sdmf)",
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

# Converts vector to key.
def vectorToKey(v):
    return round(v[0], 6), round(v[1], 6)

# Converts normal to key.
def normalToKey(n):
    return round(n[0], 6), round(n[1], 6), round(n[2], 6)

# Converts polygon to faces.
def polygonToFaces(polygon):
    faces = []
    vertices = polygon.vertices
    if len(vertices) != 4:
        faces.append([polygon.material_index, polygon.use_smooth, polygon.normal,
                     [vertices[0], vertices[2], vertices[1]]])
    else:
        faces.append([polygon.material_index, polygon.use_smooth, polygon.normal,
                     [vertices[0], vertices[2], vertices[1]]])
        faces.append([polygon.material_index, polygon.use_smooth, polygon.normal,
                     [vertices[2], vertices[0], vertices[3]]])

    return faces

# Converts texture polygon to texture faces.
def texturePolygonToTextureFaces(textureVertices):
    textureFaces = []
    if len(textureVertices) != 4:
        textureFaces.append([textureVertices[0], textureVertices[2], textureVertices[1]])
    else:
        textureFaces.append([textureVertices[0], textureVertices[2], textureVertices[1]])
        textureFaces.append([textureVertices[2], textureVertices[0], textureVertices[3]])

    return textureFaces

# Adds distinct element to the map and extends list of elements.
def addElement(mappedElements, elements, currentElementIndex, key, element):
    if key not in mappedElements:
        elements.append(element)
        mappedElements[key] = currentElementIndex
        return (currentElementIndex + 1)

    return currentElementIndex

# Represents material.
class Material(object):
    def __init__(self):
        self.faceIndex = 0
        self.shadingType = 0
        self.flags = 0
        self.colors = [[0.3, 0.3, 0.3], [1.0, 1.0, 1.0], [1.0, 1.0, 1.0]]
        self.specularLevel = 0.0
        self.glossiness = 0.0
        self.opacity = 1.0
        self.textureMapNames = ['', '', '', '']

    # Writes material to the file.
    def write(self, file):
        packedMaterial = struct.pack('<LBBffffffffffff',
                                     self.faceIndex,
                                     self.shadingType,
                                     self.flags,
                                     self.colors[0][0], self.colors[0][1], self.colors[0][2],
                                     self.colors[1][0], self.colors[1][1], self.colors[1][2],
                                     self.colors[2][0], self.colors[2][1], self.colors[2][2],
                                     self.specularLevel, self.glossiness, self.opacity)
        file.write(packedMaterial)

        for textureMapName in self.textureMapNames:
            if len(textureMapName) > 0:
                file.write(struct.pack('<H' + str(len(textureMapName)) + 's',
                                       len(textureMapName), textureMapName.encode('ascii', 'ignore')))
            else:
                file.write(struct.pack('<H', len(textureMapName)))

# Represents exception.
class SkinVertexException(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)

# Represents skin vertex.
class SkinVertex:
    def __init__(self, vertex, bonesDictionary, vertexGroupsMapping):
        self.boneIndices = []
        self.boneWeights = []

        for element in vertex.groups:
            if element.group not in vertexGroupsMapping:
                continue

            groupName = vertexGroupsMapping[element.group]
            if groupName not in bonesDictionary:
                continue

            bone = bonesDictionary[groupName]

            self.boneIndices.append(float(bone.index))
            self.boneWeights.append(element.weight)

        if len(self.boneIndices) > 4:
            print("warning: vertex " + str(vertex.index) +\
                  " is influenced by more than 4 bones (" + str(len(self.boneIndices)) + ")")

            self.boneIndices = self.boneIndices[0 : 4]
            self.boneWeights = self.boneWeights[0 : 4]
        elif len(self.boneIndices) == 0:
            raise SkinVertexException("error: vertex " + str(vertex.index) +\
                                      " is not influenced by any bones")

        totalWeight = 0.0
        for weight in self.boneWeights:
            totalWeight += weight
        normalizationFactor = 1.0 / totalWeight

        for i in range(0, len(self.boneWeights)):
            self.boneWeights[i] *= normalizationFactor

        if len(self.boneIndices) < 4:
            n = 4 - len(self.boneIndices)
            for i in range(0, n):
                self.boneIndices.append(0.0)
                self.boneWeights.append(0.0)

# Represents bone.
class Bone:
    def __init__(self, armature, rawBone):
        self.name     = rawBone.name
        self.index    = -1
        self.matrix   = (armature.matrix_world * rawBone.matrix_local).inverted()
        self.parent   = None
        self.rawBone  = rawBone
        self.children = []

    # Sets parent of the bone.
    def setParent(self, parent):
        self.parent = parent
        parent.children.append(self)

    # Writes bone to the file.
    def write(self, file):
        rotation = self.matrix.to_quaternion()
        position = self.matrix.to_translation()
        parentIndex = -1
        if self.parent != None:
            parentIndex = self.parent.index

        packedBone = struct.pack('<H' + str(len(self.name)) + 'sfffffffl',
                                 len(self.name), self.name.encode('ascii', 'ignore'),
                                 rotation.x, rotation.z, rotation.y, -rotation.w,
                                 position.x, position.z, position.y,
                                 parentIndex)
        file.write(packedBone)

# Prepares armature.
def prepareArmature(mesh, object, armature):
    # Builds hierarchy of bones.
    def buildHierarchy(bonesDictionary, rootBonesList, bonesList):
        # Enumerates bones.
        def enumerateBones(sourceBonesList, bonesList, currentBoneIndex):
            for bone in sourceBonesList:
                bone.index = currentBoneIndex
                currentBoneIndex += 1

                bonesList.append(bone)
                currentBoneIndex = enumerateBones(bone.children, bonesList, currentBoneIndex)

            return currentBoneIndex

        for bone in bonesDictionary.values():
            if bone.rawBone.parent == None or bone.rawBone.parent.name not in bonesDictionary:
                rootBonesList.append(bone)
                continue

            bone.setParent(bonesDictionary[bone.rawBone.parent.name])

        enumerateBones(rootBonesList, bonesList, 0)

    if armature == None:
        return [True, [], []]

    originalBones = armature.data.bones
    activeBoneNames = {bone.name for bone in originalBones}.intersection(
                      {group.name for group in object.vertex_groups})
    armatureBones = {bone for bone in originalBones if bone.name in activeBoneNames}

    vertexGroupsMapping = {group.index: group.name for group in object.vertex_groups if group.name in activeBoneNames}

    # build array of bones
    bonesDictionary = {bone.name: Bone(armature, bone) for bone in armatureBones}
    rootBonesList = []
    bonesList = []

    buildHierarchy(bonesDictionary, rootBonesList, bonesList)

    for i in range(0, len(bonesList)):
        if i != bonesList[i].index:
            return [False, "error: bone " + i + " has wrong index"]

    skinVertices = []
    try:
        skinVertices = [SkinVertex(vertex, bonesDictionary, vertexGroupsMapping) for vertex in mesh.vertices]
    except SkinVertexException as e:
        return [False, e.value]
    except:
        return [False, "error: skin modifier is broken"]

    return [True, bonesList, skinVertices]

# Exports mesh.
def exportMesh(mesh, object, armature, fileName):
    # validate
    if len(mesh.uv_textures) <= 0:
        return 'error: no texture coordinates'

    # get faces
    faces = []

    for f in mesh.tessfaces:
        faces.extend(polygonToFaces(f))

    # build distinct texture coordinates array and texture coordinates dictionary
    originalTexFaces = mesh.tessface_uv_textures.active.data[:]
    currentTexCoordIndex = 0
    mappedTexFaces = {}
    texCoords = []
    texFaces = []

    for texFace in originalTexFaces:
        texVertices = []
        for texCoord in texFace.uv:
            key = vectorToKey(texCoord)
            currentTexCoordIndex = addElement(mappedTexFaces, texCoords,
                                              currentTexCoordIndex, key, texCoord)
            texVertices.append(mappedTexFaces[key])

        texFaces.extend(texturePolygonToTextureFaces(texVertices))

    if len(texFaces) != len(faces):
        return 'error: wrong number of texture faces'

    # build distinct normals array
    currentNormalIndex = 0
    mappedNormals = {}
    normalFaces = []
    normals = []

    for i in range(0, len(faces)):
        isSmoothed = faces[i][1]
        normalFace = []
        if isSmoothed:
            for j in range(0, 3):
                normal = mesh.vertices[faces[i][3][j]].normal
                key = normalToKey(normal)
                currentNormalIndex = addElement(mappedNormals, normals, currentNormalIndex, key, normal)
                normalFace.append(mappedNormals[key])
        else:
            normal = faces[i][2]
            key = normalToKey(normal)
            currentNormalIndex = addElement(mappedNormals, normals, currentNormalIndex, key, normal)
            normalIndex = mappedNormals[key]
            normalFace = [normalIndex, normalIndex, normalIndex]

        normalFaces.append([normalFace[0], normalFace[1], normalFace[2]])

    if len(normalFaces) != len(faces):
        return 'error: wrong number of normal faces'

    # make ordered dictionary of faces
    sortedFaces = OrderedDict()
    materials = []

    i = 0
    for i in range(0, len(faces)):
        materialId = faces[i][0]
        if materialId not in sortedFaces:
            sortedFaces[materialId] = [[faces[i][3], normalFaces[i], texFaces[i]]]
        else:
            sortedFaces[materialId].append([faces[i][3], normalFaces[i], texFaces[i]])

    # build list of materials
    AMBIENT_MAP  = 0
    DIFFUSE_MAP  = 1
    NORMAL_MAP   = 2
    SPECULAR_MAP = 3

    faceIndex = 0
    for materialId in sortedFaces:
        material = Material()
        material.faceIndex = faceIndex

        blenderMaterial = mesh.materials[materialId]
        if blenderMaterial.specular_shader != 'PHONG' and\
           blenderMaterial.specular_shader != 'BLINN' and\
           blenderMaterial.specular_shader != 'COOKTORR':
            return 'error: material "' + blenderMaterial.name + '" has incompatible shading type (' +\
                   blenderMaterial.specular_shader + ')'

        material.shadingType = 0
        material.flags = 0

        # get ambient color
        material.colors[0] = blenderMaterial.emit * blenderMaterial.diffuse_color

        # get diffuse color
        material.colors[1] = blenderMaterial.diffuse_intensity * blenderMaterial.diffuse_color

        # get specular color
        material.colors[2] = blenderMaterial.specular_color

        material.specularLevel = blenderMaterial.specular_intensity
        material.glossiness = blenderMaterial.specular_hardness
        material.opacity = blenderMaterial.alpha

        # get texture maps
        for textureMap in blenderMaterial.texture_slots:
            if textureMap:
                textureFileName = ''

                if textureMap.texture.type != 'IMAGE':
                    print('warning: material ' + blenderMaterial.name + ' has incompatible texture type (' +\
                          textureMap.texture.type + '), empty texture shall be used')
                elif textureMap.texture.image:
                    textureFileName = textureMap.texture.image.filepath
                    slashIndex = textureFileName.rfind('\\')
                    if slashIndex >= 0:
                        textureFileName = textureFileName[slashIndex:]

                    slashIndex = textureMap.texture.image.filepath.rfind('/')

                    if slashIndex >= 0:
                        textureFileName = textureFileName[slashIndex:]

                if textureMap.use_map_color_diffuse:
                    material.textureMapNames[AMBIENT_MAP] = textureFileName
                    material.textureMapNames[DIFFUSE_MAP] = textureFileName
                elif textureMap.use_map_normal:
                    material.textureMapNames[NORMAL_MAP] = textureFileName
                elif textureMap.use_map_color_spec:
                    material.textureMapNames[SPECULAR_MAP] = textureFileName

        # add material
        materials.append(material)

        faceIndex += len(sortedFaces[materialId])

    # prepare armature
    result = prepareArmature(mesh, object, armature)
    if not result[0]:
        return result[1]

    bonesList = result[1]
    skinVertices = result[2]

    # write file
    file = open(fileName, 'wb')

    # write header
    file.write(struct.pack('<4sLLLLHH', b'SDIF', len(mesh.vertices), len(normals), len(texCoords),
                           len(faces), len(materials), len(bonesList)))

    worldMatrix = object.matrix_world
    normalsMatrix = worldMatrix.inverted().transposed().to_3x3()

    # write positions
    for vertex in mesh.vertices:
        position = worldMatrix * vertex.co
        file.write(struct.pack('<fff', position[0], position[2], position[1]))

    # write normals
    for normal in normals:
        transformedNormal = normalsMatrix * normal
        file.write(struct.pack('<fff', transformedNormal[0], transformedNormal[2], transformedNormal[1]))

    # write bone indices and weights
    for skinVertex in skinVertices:
        boneWeights = skinVertex.boneWeights
        file.write(struct.pack('<ffff', boneWeights[0], boneWeights[1], boneWeights[2], boneWeights[3]))

    for skinVertex in skinVertices:
        boneIndices = skinVertex.boneIndices
        file.write(struct.pack('<ffff', boneIndices[0], boneIndices[1], boneIndices[2], boneIndices[3]))

    # write texture coordinates
    for texCoord in texCoords:
        file.write(struct.pack('<ff', texCoord[0], 1.0 - texCoord[1]))

    # write faces, normal faces and texture faces
    for i in range(0, 3):
        for materialId in sortedFaces:
            mtlFaces = sortedFaces[materialId]
            for face in mtlFaces:
                file.write(struct.pack('<LLL',
                                       face[i][0], face[i][1], face[i][2]))

    # write materials
    for material in materials:
        material.write(file)

    # write bones
    for bone in bonesList:
        bone.write(file)

    file.flush()
    file.close()
    return 'SUCCESS'

###### EXPORT OPERATOR #######
class ExportSDMF(bpy.types.Operator, ExportHelper):
    bl_idname = "model.sdmf"
    bl_label  = "Export SDMF"

    filename_ext = ".sdmf"

    path_to_exporter = 'Path/To/Exporter.executable'

    @classmethod
    def poll(cls, context):
        return context.active_object.type in {'MESH'}

    def execute(self, context):
        print("===============================================================================")
        print('exporting mesh...')

        fileName = self.filepath
        fileName = bpy.path.ensure_ext(fileName, self.filename_ext)

        object = bpy.context.active_object

        if object.type != "MESH":
            print("error: could not export non-mesh object")
            return {'FINISHED'};

        modifiers = [modifier for modifier in object.modifiers\
                     if modifier.type == "ARMATURE" and modifier.show_viewport]

        modifier = None
        armature = None
        if len(modifiers) > 1:
            print("error: selected mesh has more than one armature modifier")
            return {'FINISHED'};
        elif len(modifiers) == 1:
            modifier = modifiers[0]
            armature = modifier.object
            modifier.show_viewport = False

        mesh = object.to_mesh(bpy.context.scene, True, "PREVIEW")
        result = exportMesh(mesh, object, armature, fileName)
        bpy.data.meshes.remove(mesh)

        if modifier != None:
            modifier.show_viewport = True

        if result == 'SUCCESS':
            subprocess.call([self.path_to_exporter, "-i", fileName, "-o", fileName])
        else:
            print(result)

        return {'FINISHED'};

### REGISTER ###
def menu_func(self, context):
    self.layout.operator(ExportSDMF.bl_idname, text = "SELENE Device mesh format (.sdmf)")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
