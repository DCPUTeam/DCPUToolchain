# Directives using the toolchain
# documentation.

import docutils

def setup(app):
    app.add_object_type("asmdirective", "asmdir");
    app.add_object_type("asminstruction", "asminst");
    app.add_object_type("ppexpressionop", "ppexprop");
    app.add_object_type("ppdirective", "ppdir");
    app.add_object_type("literal", "lit");