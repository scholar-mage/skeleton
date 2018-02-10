# vim: ft=python expandtab sw=4 ts=4 sts=4

import os
import os.path
import hashlib
import shutil
import waflib.Options
import waflib.Utils
from waflib.Configure import conf

top='.'
out='build'

build_dirs = [
    '3rdparty',
    'src',
    'tests',
]

def options(opt):
    opt.load('compiler_cxx')

    opt.add_option(
        '--variant',
        action='store',
        default='debug',
        choices=('debug', 'release'),
        help='debug or release mode')

    opt.parser.set_defaults(prefix='%s/install' % os.getcwd())

def configure(conf):
    conf.load('compiler_cxx')

    conf.env.append_value('CXXFLAGS', [
        '-Wall',
        '-Wextra',
        '-Wno-unused-variable',
        '-Wno-unused-parameter',
        '-fno-rtti',
        '-fno-exceptions',
        '-ffunction-sections',
        '-fdata-sections',
        '-Wl,--gc-sections',
        '-std=c++17',
        '-isystem', os.path.abspath('3rdparty/gtest'),
        '-isystem', os.path.abspath('3rdparty/gtest/include'),
        '-isystem', os.path.abspath('3rdparty/clara/include'),
        '-isystem', os.path.abspath('3rdparty/redi/include'),
    ])

    conf.env.append_value('INCLUDES', [
        os.path.abspath('src'),
        os.path.abspath('build/src'),
    ])

    conf.env.append_value('LIB', [
        'c',
    ])

    if conf.options.variant == 'release':
        conf.env.append_value('CXXFLAGS', [
            '-Os',
            '-DNDEBUG',
            '-ffast-math',
        ])
    else:
        conf.env.append_value('CXXFLAGS', [
            '-O0',
            '-ggdb',
        ])

def build(bld):
    bld.all_objects = { }
    bld.recurse(build_dirs)

    bld.program(
        features='cxx cxxprogram',
        source=[],
        use=bld.objects_for('core deps gtest tests'),
        target='run-tests',
        lib='pthread',
        install_path='${PREFIX}/bin')

def distclean(ctx):
    shutil.rmtree('build')
    shutil.rmtree('install')

# -----------------------------------------------
# bld.collect() extension
# -----------------------------------------------

def signature(text):
    h = hashlib.md5()
    h.update(text)
    return h.hexdigest()

@conf
def collect(ctx, groupname, sources, **kw):
    if groupname not in ctx.all_objects:
        ctx.all_objects[groupname] = [ ]

    if len(sources) == 0:
        return

    if type(sources) == str:
        sources = sources.split(' ')

    sig = groupname + '_' + signature(' '.join(sources))

    ctx.all_objects[groupname].append(sig)

    ctx.objects(
        target=sig,
        source=sources,
        **kw)

@conf
def objects_for(ctx, group_list):
    if type(group_list) == str:
        group_list = group_list.split(' ')

    result = [ ]
    for k in group_list:
        result.append(' '.join(ctx.all_objects.get(k, '')))

    return ' '.join(result)

