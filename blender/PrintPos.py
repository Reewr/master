import bpy
from mathutils import Vector

PARTS = {
  'Eye':          ( '0b1000000000000000', '0b1011111111111111' ),
  'Neck':         ( '0b0100000000000000', '0b0011111111111111' ),
  'Coxa1':        ( '0b0100100000000000', '0b1000011111111111' ),
  'ThoraxFront':  ( '0b0011000000000000', '0b1000111111111111' ),
  'Coxa2':        ( '0b0100010000000000', '0b1000101111111111' ),
  'SternumFront': ( '0b0100000000000000', '0b1011111111111111' ),
  'Sternum':      ( '0b0100000000000000', '0b1011111111111111' ),
  'SternumBack':  ( '0b0100000000000000', '0b1011111111111111' ),
  'Coxa3':        ( '0b0100001000000000', '0b1000110111111111' ),
  'ThoraxBack':   ( '0b0011000000000000', '0b1000111111111111' ),
  'Coxa4':        ( '0b0100000100000000', '0b1000111011111111' ),
  'Hip':          ( '0b0100000000000000', '0b0011111111111111' ),
  'Abdomin':      ( '0b1000000000000000', '0b1011111111111111' ),

  'TrochanterL1': ( '0b0010100000000000', '0b1001011111111111' ),
  'FemurL1':      ( '0b0010100000000000', '0b1001011111111111' ),
  'PatellaL1':    ( '0b0010100000000000', '0b1101011111111111' ),
  'TibiaL1':      ( '0b0010100000000000', '0b1101011111111111' ),
  'MetatarsusL1': ( '0b0010100000000000', '0b1101011111111111' ),
  'TarsusL1':     ( '0b0010100000000000', '0b1101011111111111' ),

  'TrochanterL2': ( '0b0010010000000000', '0b1001101111111111' ),
  'FemurL2':      ( '0b0010010000000000', '0b1001101111111111' ),
  'PatellaL2':    ( '0b0010010000000000', '0b1101101111111111' ),
  'TibiaL2':      ( '0b0010010000000000', '0b1101101111111111' ),
  'MetatarsusL2': ( '0b0010010000000000', '0b1101101111111111' ),
  'TarsusL2':     ( '0b0010010000000000', '0b1101101111111111' ),

  'TrochanterL3': ( '0b0010001000000000', '0b1001110111111111' ),
  'FemurL3':      ( '0b0010001000000000', '0b1001110111111111' ),
  'PatellaL3':    ( '0b0010001000000000', '0b1101110111111111' ),
  'TibiaL3':      ( '0b0010001000000000', '0b1101110111111111' ),
  'MetatarsusL3': ( '0b0010001000000000', '0b1101110111111111' ),
  'TarsusL3':     ( '0b0010001000000000', '0b1101110111111111' ),

  'TrochanterL4': ( '0b0010000100000000', '0b1001111011111111' ),
  'FemurL4':      ( '0b0010000100000000', '0b1001111011111111' ),
  'PatellaL4':    ( '0b0010000100000000', '0b1101111011111111' ),
  'TibiaL4':      ( '0b0010000100000000', '0b1101111011111111' ),
  'MetatarsusL4': ( '0b0010000100000000', '0b1101111011111111' ),
  'TarsusL4':     ( '0b0010000100000000', '0b1101111011111111' ),

  'TrochanterR1': ( '0b0001100000000000', '0b1010011111111111' ),
  'FemurR1':      ( '0b0001100000000000', '0b1010011111111111' ),
  'PatellaR1':    ( '0b0001100000000000', '0b1110011111111111' ),
  'TibiaR1':      ( '0b0001100000000000', '0b1110011111111111' ),
  'MetatarsusR1': ( '0b0001100000000000', '0b1110011111111111' ),
  'TarsusR1':     ( '0b0001100000000000', '0b1110011111111111' ),

  'TrochanterR2': ( '0b0001010000000000', '0b1010101111111111' ),
  'FemurR2':      ( '0b0001010000000000', '0b1010101111111111' ),
  'PatellaR2':    ( '0b0001010000000000', '0b1110101111111111' ),
  'TibiaR2':      ( '0b0001010000000000', '0b1110101111111111' ),
  'MetatarsusR2': ( '0b0001010000000000', '0b1110101111111111' ),
  'TarsusR2':     ( '0b0001010000000000', '0b1110101111111111' ),

  'TrochanterR3': ( '0b0001001000000000', '0b1010110111111111' ),
  'FemurR3':      ( '0b0001001000000000', '0b1010110111111111' ),
  'PatellaR3':    ( '0b0001001000000000', '0b1110110111111111' ),
  'TibiaR3':      ( '0b0001001000000000', '0b1110110111111111' ),
  'MetatarsusR3': ( '0b0001001000000000', '0b1110110111111111' ),
  'TarsusR3':     ( '0b0001001000000000', '0b1110110111111111' ),

  'TrochanterR4': ( '0b0001000100000000', '0b1010111011111111' ),
  'FemurR4':      ( '0b0001000100000000', '0b1010111011111111' ),
  'PatellaR4':    ( '0b0001000100000000', '0b1110111011111111' ),
  'TibiaR4':      ( '0b0001000100000000', '0b1110111011111111' ),
  'MetatarsusR4': ( '0b0001000100000000', '0b1110111011111111' ),
  'TarsusR4':     ( '0b0001000100000000', '0b1110111011111111' )
  }


def errorCheck():
  for obj in bpy.data.objects:
    if 'Imp' not in obj.name and 'Out' not in obj.name:
      continue

    for i in ['L1','L2','L3','L4','R1','R2','R3','R4']:
      if i in obj.name and i not in obj.parent.name:
        print('error in object ' + obj.name)


def showVector(v):
  if not v:
    return 'vec3(0)'
  return 'vec3(%f, %f, %f)' % (v.x, v.y, v.z)


def showPart(part):
  impAngX = bpy.data.objects.get(part + 'ImpAngX')
  impAngX = impAngX.matrix_world.to_translation() if impAngX else None

  impAngY = bpy.data.objects.get(part + 'ImpAngY')
  impAngY = impAngY.matrix_world.to_translation() if impAngY else None

  impAngZ = bpy.data.objects.get(part + 'ImpAngZ')
  impAngZ = impAngZ.matrix_world.to_translation() if impAngZ else None

  impRot  = bpy.data.objects.get(part + 'ImpRot')
  impRot = impRot.matrix_world.to_translation() if impRot else None

  output  = bpy.data.objects.get(part + 'Output')
  output = output.matrix_world.to_translation() if output else None

  xs = [
    PARTS[part][0],
    PARTS[part][1],
    showVector(impAngX),
    showVector(impAngY),
    showVector(impAngZ),
    showVector(impRot),
    showVector(output)
    ]

  return '{ "%s", {%s } }' % (part, ''.join([ '\n      %s,' % x for x in xs ]))


r = ',\n    '.join([ showPart(x) for x in PARTS if bpy.data.objects.get(x) ])
r = '  { ' + r
r += ' };'
print(r)
