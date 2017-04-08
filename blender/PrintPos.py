import bpy
from mathutils import Vector
from math import degrees

PARTS = {
  'Eye':          ( '0b1000000000000000', '0b1011111111111111',   0, False ),
  'Neck':         ( '0b0100000000000000', '0b0011111111111111',   0, False ),
  'Coxa1':        ( '0b0100100000000000', '0b1000011111111111',   0, False ),
  'ThoraxFront':  ( '0b0011000000000000', '0b1000111111111111',   0, False ),
  'Coxa2':        ( '0b0100010000000000', '0b1000101111111111',   0, False ),
  'SternumFront': ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'Sternum':      ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'SternumBack':  ( '0b0100000000000000', '0b1011111111111111',   0, False ),
  'Coxa3':        ( '0b0100001000000000', '0b1000110111111111',   0, False ),
  'ThoraxBack':   ( '0b0011000000000000', '0b1000111111111111',   0, False ),
  'Coxa4':        ( '0b0100000100000000', '0b1000111011111111',   0, False ),
  'Hip':          ( '0b0100000000000000', '0b0011111111111111',   0, False ),
  'Abdomin':      ( '0b1000000000000000', '0b1011111111111111',   0, False ),

  'TrochanterL1': ( '0b0010100000000000', '0b1001011111111111',  30, True  ),
  'FemurL1':      ( '0b0010100000000000', '0b1001011111111111',  35, True  ),
  'PatellaL1':    ( '0b0010100000000000', '0b1101011111111111', -45, True  ),
  'TibiaL1':      ( '0b0010100000000000', '0b1101011111111111', -45, True  ),
  'MetatarsusL1': ( '0b0010100000000000', '0b1101011111111111',   0, False ),
  'TarsusL1':     ( '0b0010100000000000', '0b1101011111111111', -20, False ),

  'TrochanterL2': ( '0b0010010000000000', '0b1001101111111111',  10, True  ),
  'FemurL2':      ( '0b0010010000000000', '0b1001101111111111',  35, True  ),
  'PatellaL2':    ( '0b0010010000000000', '0b1101101111111111', -45, True  ),
  'TibiaL2':      ( '0b0010010000000000', '0b1101101111111111', -45, True  ),
  'MetatarsusL2': ( '0b0010010000000000', '0b1101101111111111',   0, False ),
  'TarsusL2':     ( '0b0010010000000000', '0b1101101111111111', -20, False ),

  'TrochanterL3': ( '0b0010001000000000', '0b1001110111111111', -15, True  ),
  'FemurL3':      ( '0b0010001000000000', '0b1001110111111111',  35, True  ),
  'PatellaL3':    ( '0b0010001000000000', '0b1101110111111111', -45, True  ),
  'TibiaL3':      ( '0b0010001000000000', '0b1101110111111111', -45, True  ),
  'MetatarsusL3': ( '0b0010001000000000', '0b1101110111111111',   0, False ),
  'TarsusL3':     ( '0b0010001000000000', '0b1101110111111111', -20, False ),

  'TrochanterL4': ( '0b0010000100000000', '0b1001111011111111', -40, True  ),
  'FemurL4':      ( '0b0010000100000000', '0b1001111011111111',  35, True  ),
  'PatellaL4':    ( '0b0010000100000000', '0b1101111011111111', -45, True  ),
  'TibiaL4':      ( '0b0010000100000000', '0b1101111011111111', -45, True  ),
  'MetatarsusL4': ( '0b0010000100000000', '0b1101111011111111',   0, False ),
  'TarsusL4':     ( '0b0010000100000000', '0b1101111011111111', -20, False ),

  'TrochanterR1': ( '0b0001100000000000', '0b1010011111111111',  30, True  ),
  'FemurR1':      ( '0b0001100000000000', '0b1010011111111111',  35, True  ),
  'PatellaR1':    ( '0b0001100000000000', '0b1110011111111111', -45, True  ),
  'TibiaR1':      ( '0b0001100000000000', '0b1110011111111111', -45, True  ),
  'MetatarsusR1': ( '0b0001100000000000', '0b1110011111111111',   0, False ),
  'TarsusR1':     ( '0b0001100000000000', '0b1110011111111111', -20, False ),

  'TrochanterR2': ( '0b0001010000000000', '0b1010101111111111',  10, True  ),
  'FemurR2':      ( '0b0001010000000000', '0b1010101111111111',  35, True  ),
  'PatellaR2':    ( '0b0001010000000000', '0b1110101111111111', -45, True  ),
  'TibiaR2':      ( '0b0001010000000000', '0b1110101111111111', -45, True  ),
  'MetatarsusR2': ( '0b0001010000000000', '0b1110101111111111',   0, False ),
  'TarsusR2':     ( '0b0001010000000000', '0b1110101111111111', -20, False ),

  'TrochanterR3': ( '0b0001001000000000', '0b1010110111111111', -15, True  ),
  'FemurR3':      ( '0b0001001000000000', '0b1010110111111111',  35, True  ),
  'PatellaR3':    ( '0b0001001000000000', '0b1110110111111111', -45, True  ),
  'TibiaR3':      ( '0b0001001000000000', '0b1110110111111111', -45, True  ),
  'MetatarsusR3': ( '0b0001001000000000', '0b1110110111111111',   0, False ),
  'TarsusR3':     ( '0b0001001000000000', '0b1110110111111111', -20, False ),

  'TrochanterR4': ( '0b0001000100000000', '0b1010111011111111', -40, True  ),
  'FemurR4':      ( '0b0001000100000000', '0b1010111011111111',  35, True  ),
  'PatellaR4':    ( '0b0001000100000000', '0b1110111011111111', -45, True  ),
  'TibiaR4':      ( '0b0001000100000000', '0b1110111011111111', -45, True  ),
  'MetatarsusR4': ( '0b0001000100000000', '0b1110111011111111',   0, False ),
  'TarsusR4':     ( '0b0001000100000000', '0b1110111011111111', -20, False )
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
  xs = [ PARTS[name][0], PARTS[name][1], 'radians(%i)' % PARTS[name][2], 'true' if PARTS[name][3] else 'false' ]
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

  impAngX = bpy.data.objects.get(name + 'ImpAngX')
  if impAngX:
    xs.append({ name+'ImpAngX': impAngX.matrix_world.to_translation() })

  impAngY = bpy.data.objects.get(name + 'ImpAngY')
  if impAngY:
    xs.append({ name+'ImpAngY': impAngY.matrix_world.to_translation() })

  impAngZ = bpy.data.objects.get(name + 'ImpAngZ')
  if impAngZ:
    xs.append({ name+'ImpAngZ': impAngZ.matrix_world.to_translation() })

  # impRot  = bpy.data.objects.get(name + 'ImpRot')
  # if impRot:
  #   xs.append({ name+'ImpRot': impRot.matrix_world.to_translation() })

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

  # inn += [ getInputLegCenterNeurons() ]

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
