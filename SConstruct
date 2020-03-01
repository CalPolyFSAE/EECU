#!/bin/python

BSP_PATH = '../MKELibrary/'

import os

GNU_PATH = SConscript(BSP_PATH+'config.txt')

compileTarget = 'main'

# Create Communal build directory to store all the .o's
VariantDir('build/board', 'board')
VariantDir('build/source', 'source')
VariantDir('build/rtos', 'rtos')

env = Environment(ENV = os.environ)

env['AR'] = GNU_PATH+'arm-none-eabi-ar'
env['AS'] = GNU_PATH+'arm-none-eabi-gcc'
env['ASCOM'] = '$AS $ASFLAGS $_CPPINCFLAGS -o $TARGET $SOURCES'
env['CC'] = GNU_PATH+'arm-none-eabi-gcc'
env['CXX'] = GNU_PATH+'arm-none-eabi-g++'
env['CXXCOM'] = '$CXX -o $TARGET -c $CXXFLAGS $_CCCOMCOM $SOURCES'
env['LINK'] = GNU_PATH+'arm-none-eabi-g++'                
env['RANLIB'] = GNU_PATH+'arm-none-eabi-ranlib'
env['OBJCOPY'] = GNU_PATH+'arm-none-eabi-objcopy'
env['PROGSUFFIX'] = '.elf'
OBJDUMP = GNU_PATH+'arm-none-eabi-objdump'
READELF = GNU_PATH+'arm-none-eabi-readelf'


env['ASFLAGS'] = '-g -DDEBUG -D__STARTUP_CLEAR_BSS \
        -D__STARTUP_INITIALIZE_NONCACHEDATA -Wall -fno-common \
        -ffunction-sections -fdata-sections -ffreestanding \
        -fno-builtin -mthumb -mapcs -std=gnu99 -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16'
env['CCFLAGS'] = '-O0 -g -DDEBUG -DCPU_MKE18F512VLH16 \
        -DTWR_KE18F -DTOWER -Wall -fno-common \
        -ffunction-sections -fdata-sections -ffreestanding \
        -fno-builtin -mthumb -mapcs -std=gnu99 -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP'
env['CXXFLAGS'] = '-O0 -g -DDEBUG -Wall \
        -fno-common -ffunction-sections -fdata-sections \
        -ffreestanding -fno-builtin -mthumb -mapcs \
        -fno-rtti -fno-exceptions -mcpu=cortex-m4 \
        -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP \
        -DCPU_MKE18F512VLH16'

includes = [
    'source',
    'board',
    BSP_PATH+'CMSIS',
    BSP_PATH+'drivers',
    BSP_PATH+'utilities',
    BSP_PATH+'lib',
    BSP_PATH+'System',
    BSP_PATH
]


rtos_includes = [
    'rtos',
    'rtos/include',
    'rtos/portable/ARM_CM4F'
    ]

env.Append(CPPPATH = rtos_includes)
env.Append(CPPPATH = includes)

env['LINKFLAGS'] = '-O0 -g -DDEBUG -Wall \
    -fno-common -ffunction-sections -fdata-sections \
    -ffreestanding -fno-builtin -mthumb -mapcs \
    -fno-rtti -fno-exceptions -mcpu=cortex-m4 \
    -mfloat-abi=hard -mfpu=fpv4-sp-d16 -MMD -MP \
    -DCPU_MKE18F512VLH16 -specs=nano.specs \
    -specs=nosys.specs -Wall -fno-common \
    -ffunction-sections -fdata-sections \
    -ffreestanding -fno-builtin -mthumb \
    -mapcs -Xlinker --gc-sections -Xlinker \
    -static -Xlinker -z -Xlinker muldefs \
    -mcpu=cortex-m4 -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 -TMKE18F512xxx16_flash.ld -static'

src = \
    Glob('build/board/*.c'), \
    Glob('build/source/*.cpp'), \
    Glob('build/source/*.c'),
    
rtos_src = \
    Glob('build/rtos/*.c'), \
    'build/rtos/portable/ARM_CM4F/port.c', \
    'build/rtos/portable/MemMang/heap_4.c'

# Run the compile command and create .elf
env.Program(compileTarget, source=src+rtos_src, LIBS=['bsp'], LIBPATH=[BSP_PATH])

# Create .lst from .elf
#env.Command(compileTarget+".lst", compileTarget+".elf", \
#    OBJDUMP+" -D " + compileTarget+".elf" + " > " + compileTarget+".lst")

# Print Memory Map -> .elf Headers
#env.Command(compileTarget, compileTarget+".elf", READELF+" -e " + compileTarget+".elf")
