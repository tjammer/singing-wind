import os
import bpy
import sys
sys.path.append('.')
sys.path.append('../external/flatbuffers/python')
import flatbuffers
from Map import Level, Face, Vertex

if __name__ == "__main__":
    print(os.getcwd())
    bpy.ops.wm.open_mainfile(filepath="../assets/raw/test.blend")
    builder = flatbuffers.Builder(1)
    for obj in bpy.data.objects:
        if obj.name != "Collision":
            continue
        bpy.context.scene.objects.active = obj
        bpy.ops.object.modifier_apply(modifier="Triangulate")
        obdata = obj.data

        vertices = [(v.co.x, v.co.y) for v in obdata.vertices]
        faces = [tuple(v for v in f.vertices) for f in obdata.polygons]

        Level.LevelStartVertsVector(builder, len(vertices))
        for v in reversed(vertices):
            Vertex.CreateVertex(builder, *v)
        verts = builder.EndVector(len(vertices))

        Level.LevelStartFacesVector(builder, len(faces))
        for f in reversed(faces):
            Face.CreateFace(builder, *f)
        facs = builder.EndVector(len(faces))

        Level.LevelStart(builder)
        Level.LevelAddVerts(builder, verts)
        Level.LevelAddFaces(builder, facs)
        level = Level.LevelEnd(builder)
        builder.Finish(level)

        with open('test.swmap', 'wb') as f:
            f.write(builder.Output())

