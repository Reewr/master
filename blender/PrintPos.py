import bpy
from mathutils import Vector
from math import degrees

PARTS = {
  'Eye':          ( '0b1000000000000000', '0b1011111111111111',   0 ),
  'Neck':         ( '0b0100000000000000', '0b0011111111111111',   0 ),
  'Coxa1':        ( '0b0100100000000000', '0b1000011111111111',   0 ),
  'ThoraxFront':  ( '0b0011000000000000', '0b1000111111111111',   0 ),
  'Coxa2':        ( '0b0100010000000000', '0b1000101111111111',   0 ),
  'SternumFront': ( '0b0100000000000000', '0b1011111111111111',   0 ),
  'Sternum':      ( '0b0100000000000000', '0b1011111111111111',   0 ),
  'SternumBack':  ( '0b0100000000000000', '0b1011111111111111',   0 ),
  'Coxa3':        ( '0b0100001000000000', '0b1000110111111111',   0 ),
  'ThoraxBack':   ( '0b0011000000000000', '0b1000111111111111',   0 ),
  'Coxa4':        ( '0b0100000100000000', '0b1000111011111111',   0 ),
  'Hip':          ( '0b0100000000000000', '0b0011111111111111',   0 ),
  'Abdomin':      ( '0b1000000000000000', '0b1011111111111111',   0 ),

  'TrochanterL1': ( '0b0010100000000000', '0b1001011111111111',  30 ),
  'FemurL1':      ( '0b0010100000000000', '0b1001011111111111',  35 ),
  'PatellaL1':    ( '0b0010100000000000', '0b1101011111111111', -45 ),
  'TibiaL1':      ( '0b0010100000000000', '0b1101011111111111', -45 ),
  'MetatarsusL1': ( '0b0010100000000000', '0b1101011111111111',   0 ),
  'TarsusL1':     ( '0b0010100000000000', '0b1101011111111111', -20 ),

  'TrochanterL2': ( '0b0010010000000000', '0b1001101111111111',  10 ),
  'FemurL2':      ( '0b0010010000000000', '0b1001101111111111',  35 ),
  'PatellaL2':    ( '0b0010010000000000', '0b1101101111111111', -45 ),
  'TibiaL2':      ( '0b0010010000000000', '0b1101101111111111', -45 ),
  'MetatarsusL2': ( '0b0010010000000000', '0b1101101111111111',   0 ),
  'TarsusL2':     ( '0b0010010000000000', '0b1101101111111111', -20 ),

  'TrochanterL3': ( '0b0010001000000000', '0b1001110111111111', -15 ),
  'FemurL3':      ( '0b0010001000000000', '0b1001110111111111',  35 ),
  'PatellaL3':    ( '0b0010001000000000', '0b1101110111111111', -45 ),
  'TibiaL3':      ( '0b0010001000000000', '0b1101110111111111', -45 ),
  'MetatarsusL3': ( '0b0010001000000000', '0b1101110111111111',   0 ),
  'TarsusL3':     ( '0b0010001000000000', '0b1101110111111111', -20 ),

  'TrochanterL4': ( '0b0010000100000000', '0b1001111011111111', -40 ),
  'FemurL4':      ( '0b0010000100000000', '0b1001111011111111',  35 ),
  'PatellaL4':    ( '0b0010000100000000', '0b1101111011111111', -45 ),
  'TibiaL4':      ( '0b0010000100000000', '0b1101111011111111', -45 ),
  'MetatarsusL4': ( '0b0010000100000000', '0b1101111011111111',   0 ),
  'TarsusL4':     ( '0b0010000100000000', '0b1101111011111111', -20 ),

  'TrochanterR1': ( '0b0001100000000000', '0b1010011111111111',  30 ),
  'FemurR1':      ( '0b0001100000000000', '0b1010011111111111',  35 ),
  'PatellaR1':    ( '0b0001100000000000', '0b1110011111111111', -45 ),
  'TibiaR1':      ( '0b0001100000000000', '0b1110011111111111', -45 ),
  'MetatarsusR1': ( '0b0001100000000000', '0b1110011111111111',   0 ),
  'TarsusR1':     ( '0b0001100000000000', '0b1110011111111111', -20 ),

  'TrochanterR2': ( '0b0001010000000000', '0b1010101111111111',  10 ),
  'FemurR2':      ( '0b0001010000000000', '0b1010101111111111',  35 ),
  'PatellaR2':    ( '0b0001010000000000', '0b1110101111111111', -45 ),
  'TibiaR2':      ( '0b0001010000000000', '0b1110101111111111', -45 ),
  'MetatarsusR2': ( '0b0001010000000000', '0b1110101111111111',   0 ),
  'TarsusR2':     ( '0b0001010000000000', '0b1110101111111111', -20 ),

  'TrochanterR3': ( '0b0001001000000000', '0b1010110111111111', -15 ),
  'FemurR3':      ( '0b0001001000000000', '0b1010110111111111',  35 ),
  'PatellaR3':    ( '0b0001001000000000', '0b1110110111111111', -45 ),
  'TibiaR3':      ( '0b0001001000000000', '0b1110110111111111', -45 ),
  'MetatarsusR3': ( '0b0001001000000000', '0b1110110111111111',   0 ),
  'TarsusR3':     ( '0b0001001000000000', '0b1110110111111111', -20 ),

  'TrochanterR4': ( '0b0001000100000000', '0b1010111011111111', -40 ),
  'FemurR4':      ( '0b0001000100000000', '0b1010111011111111',  35 ),
  'PatellaR4':    ( '0b0001000100000000', '0b1110111011111111', -45 ),
  'TibiaR4':      ( '0b0001000100000000', '0b1110111011111111', -45 ),
  'MetatarsusR4': ( '0b0001000100000000', '0b1110111011111111',   0 ),
  'TarsusR4':     ( '0b0001000100000000', '0b1110111011111111', -20 )
  }


def errorCheck():
  for obj in bpy.data.objects:
    if 'Imp' not in obj.name and 'Out' not in obj.name:
      continue

    for i in ['L1','L2','L3','L4','R1','R2','R3','R4']:
      if i in obj.name and i not in obj.parent.name:
        print('error in object ' + obj.name)


def showVec(vec):
  for k,v in vec.items():
    if not v:
      return 'vec3(0)'
    return 'vec3(%f, %f, %f)' % (v.x, v.y, v.z)

def showVector(vec):
  for k,v in vec.items():
    if not v:
      return ''
    return '{ %f, %f, %f }' % (v.x, v.y, v.z)


def showPart(name):
  xs = [ PARTS[name][0], PARTS[name][1], 'radians(%i)' % PARTS[name][2] ]
  return '{ "%s", { %s } }' % (name, ', '.join(xs))

def printParts():
  r = ',\n    '.join([ showPart(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ])
  r = '  { ' + r
  r += ' };'
  print('std::map<std::string, Spider::Part> Spider::SPIDER_PARTS =')
  print(r)

def getInputLegCenterNeurons():
  xs = []

  for j in ['L','R']:
    for i in [1,2,3,4]:
      name = j + str(i) + 'Center'
      x = bpy.data.objects.get(name).matrix_world.to_translation()
      print(name, x)
      xs.append({ name: x })

  return xs;

def getInputNeurons(name):
  xs = []

  # impAngX = bpy.data.objects.get(name + 'ImpAngX')
  # if impAngX:
  #   xs.append({ name+'ImpAngX': impAngX.matrix_world.to_translation() })

  # impAngY = bpy.data.objects.get(name + 'ImpAngY')
  # if impAngY:
  #   xs.append({ name+'ImpAngY': impAngY.matrix_world.to_translation() })

  # impAngZ = bpy.data.objects.get(name + 'ImpAngZ')
  # if impAngZ:
  #   xs.append({ name+'ImpAngZ': impAngZ.matrix_world.to_translation() })

  impRot  = bpy.data.objects.get(name + 'ImpRot')
  if impRot:
    xs.append({ name+'ImpRot': impRot.matrix_world.to_translation() })

  return xs;

def getOutputNeurons(name):
  xs = []

  output  = bpy.data.objects.get(name + 'Output')
  if output:
    xs.append({ name+'Output': output.matrix_world.to_translation() })

  return xs

def printNeuronData():
  inn = []
  out = []

  inn += [ getInputLegCenterNeurons() ]

  inn += [ getInputNeurons(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ]
  out += [ getOutputNeurons(x) for x in sorted(PARTS) if bpy.data.objects.get(x) ]

  # flatten lists
  inn = [ '    ' + showVector(x) for xs in inn for x in xs if len(xs) > 0 ]
  out = [ '    ' + showVector(x) for xs in out for x in xs if len(xs) > 0 ]

  print('  std::vector<std::vector<double>> inputs{')
  print(',\n'.join(inn))
  print('  };')
  print('  std::vector<std::vector<double>> hidden{};')
  print('  std::vector<std::vector<double>> outputs{')
  print(',\n'.join(out))
  print('  };')

errorCheck()
printParts()
printNeuronData()
