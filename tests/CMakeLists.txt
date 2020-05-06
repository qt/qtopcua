# Generated from tests.pro.

if(QT_BUILD_STANDALONE_TESTS)
    # Add qt_find_package calls for extra dependencies that need to be found when building
    # the standalone tests here.
endif()
qt_build_tests()

# special case begin
if(QT_FEATURE_open62541)
    add_subdirectory(open62541-testserver)
endif()
# special case end
