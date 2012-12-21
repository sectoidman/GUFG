Execute(Delete("dist"))
Mkdir(".config")
Mkdir(".data")

VariantDir('.build', 'src')
SConscript('src/SConscript.py', variant_dir='.build')
