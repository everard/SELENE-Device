# Copyright (c) 2012 Nezametdinov Ildus
# Licensed under the MIT License (see LICENSE.txt for details)

bl_info = {
    "name": "SELENE Device model format (.sle)",
    "author": "Nezametdinov Ildus (everard)",
    "version": (1, 0, 0),
    "blender": (2, 6, 3),
    "location": "File > Export > SELENE Device (.sle)",
    "description": "Export SELENE Device models (.sle)",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Import-Export"}

import bpy
from bpy_extras.io_utils import ExportHelper
from collections import OrderedDict
import subprocess
import struct

# Converts vector to key
def vectorToKey(v):
    return round(v[0], 6), round(v[1], 6)

# Converts polygon to faces
def polygonToFaces(polygon):
    faces = []
    vertices = polygon.vertices
    if len(vertices) != 4:
        faces.append([polygon.material_index, [vertices[0], vertices[1], vertices[2]]])
    else:
        faces.append([polygon.material_index, [vertices[0], vertices[1], vertices[2]]])
        faces.append([polygon.material_index, [vertices[2], vertices[3], vertices[0]]])

    return faces

# Converts texture polygon to texture faces
def texturePolygonToTextureFaces(textureVertices):
    textureFaces = []
    if len(textureVertices) != 4:
        textureFaces.append([textureVertices[0], textureVertices[1], textureVertices[2]])
    else:
        textureFaces.append([textureVertices[0], textureVertices[1], textureVertices[2]])
        textureFaces.append([textureVertices[2], textureVertices[3], textureVertices[0]])

    return textureFaces

# Represents material
class Material(object):
    # Constructs material
    def __init__(self):
        self.faceIndex = 0
        self.type = 'PHONG'
        self.isTwoSided = 0
        self.colors = [[0.3, 0.3, 0.3], [1.0, 1.0, 1.0], [1.0, 1.0, 1.0]]
        self.specularLevel = 0.0
        self.glossiness = 0.0
        self.opacity = 1.0
        self.textureMapNames = ['', '', '', '']

    # Packs material to structure
    def write(self, file):
        packedMaterial = struct.pack('<L5sBffffffffffff',
                                     self.faceIndex,
                                     self.type.encode('ascii', 'ignore'),
                                     self.isTwoSided,
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

# Exports mesh
def exportMesh(mesh, fileName):
    # validate
    if len(mesh.uv_textures) <= 0:
        return 'ERROR: Texture coordinates must be specified'

    # get faces
    faces = []

    for f in mesh.tessfaces:
        faces.extend(polygonToFaces(f))

    # build distinct texture coordinates array and texture coordinates dictionary
    originalTexFaces = mesh.tessface_uv_textures.active.data[:]
    mappedTexFaces = {}
    texCoords = []

    currentTexCoordIndex = 0
    for texFace in originalTexFaces:
        for texCoord in texFace.uv:
            key = vectorToKey(texCoord)

            if key not in mappedTexFaces:
                mappedTexFaces[key] = currentTexCoordIndex
                texCoords.append(texCoord)
                currentTexCoordIndex += 1

    # get texture faces
    texFaces = []
    for texFace in originalTexFaces:
        texVertices = []
        for texCoord in texFace.uv:
            key = vectorToKey(texCoord)

            if key not in mappedTexFaces:
                return 'ERROR: Wrong texture coordinates'

            texVertices.append(mappedTexFaces[key])

        texFaces.extend(texturePolygonToTextureFaces(texVertices))

    if len(texFaces) != len(faces):
        return 'ERROR: Texture faces and faces mismatch'

    # make ordered dictionary of faces
    sortedFaces = OrderedDict()
    materials = []

    i = 0
    for i in range(0, len(faces)):
        materialId = faces[i][0]
        if materialId not in sortedFaces:
            sortedFaces[materialId] = [[faces[i][1], texFaces[i]]]
        else:
            sortedFaces[materialId].append([faces[i][1], texFaces[i]])

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
        if blenderMaterial.specular_shader != 'PHONG' and blenderMaterial.specular_shader != 'BLINN':
            return 'ERROR: Material ' + blenderMaterial.name + ' has wrong shading type (' +\
                   blenderMaterial.specular_shader + ')'

        material.type = blenderMaterial.specular_shader
        material.isTwoSided = 0

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
                if textureMap.texture.type != 'IMAGE':
                    return 'ERROR: Material ' + blenderMaterial.name + ' has wrong texture type (' +\
                           textureMap.texture.type + '), only images are allowed'

                textureFileName = ''
                if textureMap.texture.image:
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

        print(vars(material))

        # add material
        materials.append(material)

        faceIndex += len(sortedFaces[materialId])

    # write file
    file = open(fileName, 'wb')

    # write header
    file.write(struct.pack('<7sLLL', b'TEMPSLE', len(mesh.vertices), len(faces), len(texCoords)))

    # write positions
    for vertex in mesh.vertices:
        file.write(struct.pack('<fff', vertex.co[0], vertex.co[1], vertex.co[2]))

    # write texture coordinates
    for texCoord in texCoords:
        file.write(struct.pack('<ff', texCoord[0], texCoord[1]))

    # write faces and texture faces
    for materialId in sortedFaces:
        mtlFaces = sortedFaces[materialId]
        for face in mtlFaces:
            file.write(struct.pack('<LLLLLL',
                                   face[0][0], face[0][1], face[0][2],
                                   face[1][0], face[1][1], face[1][2]))

    # write materials
    file.write(struct.pack('<L', len(materials)))

    for material in materials:
        material.write(file)

    # write skin vertices
    file.write(struct.pack('<L', 0))

    file.flush()
    file.close()
    return 'SUCCESS'

###### EXPORT OPERATOR #######
class ExportSLE(bpy.types.Operator, ExportHelper):
    bl_idname = "model.sle"
    bl_label = "Export SLE"

    filename_ext = ".sle"

    path_to_exporter = 'Path To Exporter.exe'

    @classmethod
    def poll(cls, context):
        return context.active_object.type in {'MESH'}

    def execute(self, context):
        print('Exporting SELENE Device mesh...')

        fileName = self.filepath
        fileName = bpy.path.ensure_ext(fileName, self.filename_ext)

        object = bpy.context.active_object
        newObject = object.copy()
        for modifier in [modifier for modifier in newObject.modifiers if modifier.type == "ARMATURE"]:
            newObject.modifiers.remove(modifier)

        mesh = newObject.to_mesh(bpy.context.scene, True, "PREVIEW")
        result = exportMesh(mesh, fileName)

        bpy.data.objects.remove(newObject)
        bpy.data.meshes.remove(mesh)

        if result == 'SUCCESS':
            fileName = fileName.replace(' ', '?')
            subprocess.call([self.path_to_exporter + '/Exporter.exe', fileName, fileName])
        else:
            print(result)

        return {'FINISHED'};

### REGISTER ###
def menu_func(self, context):
    self.layout.operator(ExportSLE.bl_idname, text = "SELENE Device model (.sle)")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
