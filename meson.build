project('tina', 'c', default_options: ['c_std=c99'])

# concord_deps = [dependency('concord'), dependency('threads'), dependency('libcurl')] # forgot that the .pc file was my own
concord_deps = [dependency('threads'), dependency('libcurl')]
jansson = dependency('jansson')

src = ['src/main.c', 'src/intents.c', 'src/env.c', 'src/levenshtein.c']

# executable('tina', sources : src, dependencies : [concord_deps, jansson], link_args : ['-ldotenv-s']) # account for the earlier mistake
executable('tina', sources : src, dependencies : [concord_deps, jansson], link_args : ['-ldotenv-s', '-ldiscord'])
