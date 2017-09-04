{
  "includes": [
    "ext/snowcrash/common.gypi"
  ],

  "targets" : [

# LIBSOS
    {
      'target_name': 'libsos',
      'type': 'static_library',
      'direct_dependent_settings' : {
          'include_dirs': [ 'ext/sos/src' ],
      },
      'sources': [
        'ext/sos/src/sos.cc',
        'ext/sos/src/sos.h',
        'ext/sos/src/sosJSON.h',
        'ext/sos/src/sosYAML.h'
      ]
    },

# LIBDRAFTER  
    {
      "target_name": "libdrafter",
      'type': '<(libdrafter_type)',
      "conditions" : [
        [ 'libdrafter_type=="shared_library"', { 'defines' : [ 'DRAFTER_BUILD_SHARED' ] }, { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      'direct_dependent_settings' : {
          'include_dirs': [
            'src',
          ],
      },
      'export_dependent_settings': [
        'libsos',
        'ext/snowcrash/snowcrash.gyp:libsnowcrash',
      ],
      "sources": [
        "src/drafter.h",
        "src/drafter.cc",
        "src/stream.h",
        "src/Version.h",

        "src/NodeInfo.h",
        "src/Serialize.h",
        "src/Serialize.cc",
        "src/SerializeResult.h",
        "src/SerializeResult.cc",
        "src/RefractAPI.h",
        "src/RefractAPI.cc",
        "src/RefractDataStructure.h",
        "src/RefractDataStructure.cc",
        "src/RefractSourceMap.h",
        "src/RefractSourceMap.cc",
        "src/Render.h",
        "src/Render.cc",
        "src/NamedTypesRegistry.cc",
        "src/NamedTypesRegistry.h",
        "src/RefractElementFactory.h",
        "src/RefractElementFactory.cc",
        "src/ConversionContext.cc",
        "src/ConversionContext.h",

# librefract parts - will be separated into other project
        "src/refract/ElementIfc.h",
        "src/refract/MemberElementCollection.h",
        "src/refract/MemberElementCollection.cc",
        "src/refract/Element.h",
        "src/refract/Element.cc",
        "src/refract/ElementFwd.h",

        "src/refract/Visitor.h",

        "src/refract/VisitorUtils.h",
        "src/refract/VisitorUtils.cc",

        "src/refract/SerializeCompactVisitor.h",
        "src/refract/SerializeCompactVisitor.cc",
        "src/refract/SerializeVisitor.h",
        "src/refract/SerializeVisitor.cc",
        "src/refract/ComparableVisitor.h",
        "src/refract/ComparableVisitor.cc",
        "src/refract/TypeQueryVisitor.h",
        "src/refract/TypeQueryVisitor.cc",
        "src/refract/IsExpandableVisitor.h",
        "src/refract/IsExpandableVisitor.cc",
        "src/refract/ExpandVisitor.h",
        "src/refract/ExpandVisitor.cc",
        "src/refract/RenderJSONVisitor.h",
        "src/refract/RenderJSONVisitor.cc",
        "src/refract/PrintVisitor.h",
        "src/refract/PrintVisitor.cc",
        "src/refract/JSONSchemaVisitor.h",
        "src/refract/JSONSchemaVisitor.cc",
        "src/refract/FilterVisitor.h",

        "src/refract/Registry.h",
        "src/refract/Registry.cc",

        "src/refract/Build.h",

        "src/refract/AppendDecorator.h",
        "src/refract/ElementInserter.h",
        "src/refract/Query.h",
        "src/refract/Query.cc",
        "src/refract/Iterate.h",

        "src/refract/data/NamedElements.h",
        "src/refract/data/ElementFwd.h",
        "src/refract/data/ElementIfc.h",
        "src/refract/data/Element.h",
        "src/refract/data/Element2.cc",
        "src/refract/data/Array.h",
        "src/refract/data/Bool.h",
        "src/refract/data/Enum.h",
        "src/refract/data/Extend.h",
        "src/refract/data/Holder.h",
        "src/refract/data/Member.h",
        "src/refract/data/Null.h",
        "src/refract/data/Number.h",
        "src/refract/data/Object.h",
        "src/refract/data/Option.h",
        "src/refract/data/Ref.h",
        "src/refract/data/Select.h",
        "src/refract/data/String.h",
        "src/refract/data/Traits.h",
        "src/refract/data/TypeQueryVisitor2.h",
        "src/refract/data/Visitor.h",

        "src/refract/data/NamedElements.cc",
        "src/refract/data/Array.cc",
        "src/refract/data/Bool.cc",
        "src/refract/data/Enum.cc",
        "src/refract/data/Extend.cc",
        "src/refract/data/Holder.cc",
        "src/refract/data/Member.cc",
        "src/refract/data/Null.cc",
        "src/refract/data/Number.cc",
        "src/refract/data/Object.cc",
        "src/refract/data/Option.cc",
        "src/refract/data/Ref.cc",
        "src/refract/data/Select.cc",
        "src/refract/data/String.cc",
        "src/refract/data/TypeQueryVisitor2.cc",
      ],
      "dependencies": [
        "libsos",
        "ext/snowcrash/snowcrash.gyp:libsnowcrash",
      ],
    },

# TESTLIBDRAFTER
    {
      'target_name': 'test-libdrafter',
      'type': 'executable',
      'include_dirs': [
        'test/vendor/Catch/include',
        'test/vendor/dtl/dtl',
        'src/refract',
      ],
      'sources': [
        "test/test-drafter.cc",

        "test/refract/data/test-Array.cc",
        "test/refract/data/test-Bool.cc",
        "test/refract/data/test-Element.cc",
        "test/refract/data/test-Enum.cc",
        "test/refract/data/test-Extend.cc",
        "test/refract/data/test-Holder.cc",
        "test/refract/data/test-Member.cc",
        "test/refract/data/test-Null.cc",
        "test/refract/data/test-Number.cc",
        "test/refract/data/test-Object.cc",
        "test/refract/data/test-Option.cc",
        "test/refract/data/test-Ref.cc",
        "test/refract/data/test-Select.cc",
        "test/refract/data/test-String.cc",

        "test/test-MemberElementCollection.cc",
        "test/test-RefractDataStructureTest.cc",
        "test/test-RefractAPITest.cc",
        "test/test-RefractParseResultTest.cc",
        "test/test-RenderTest.cc",
        "test/test-RefractSourceMapTest.cc",
        "test/test-SchemaTest.cc",
        "test/test-CircularReferenceTest.cc",
        "test/test-ApplyVisitorTest.cc",
        "test/test-ExtendElementTest.cc",
        "test/test-ElementFactoryTest.cc",
        "test/test-OneOfTest.cc",
        "test/test-SyntaxIssuesTest.cc",
      ],
      'dependencies': [
        "libdrafter",
      ],
      'conditions': [
         [ 'OS=="win"', { 'defines' : [ 'WIN' ] } ]
      ],
    },

# DRAFTER    
    {
      "target_name": "drafter",
      "type": "executable",
      "conditions" : [
        [ 'libdrafter_type=="static_library"', { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      "sources": [
        "src/main.cc",
        "src/config.cc",
        "src/config.h",
        "src/reporting.cc",
        "src/reporting.h",
      ],
      "include_dirs": [
        "ext/cmdline",
      ],
      "dependencies": [
        "libdrafter",
      ],
    },

# DRAFTER C-API TEST
    {
      "target_name": "test-capi",
      "type": "executable",
      "conditions" : [
        [ 'libdrafter_type=="static_library"', { 'defines' : [ 'DRAFTER_BUILD_STATIC' ] }],
      ],
      "sources": [
          "test/test-CAPI.c"
      ],
      "dependencies": [
        "libdrafter",
      ],
    },
  ],
}
