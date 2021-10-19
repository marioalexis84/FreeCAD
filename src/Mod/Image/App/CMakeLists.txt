if(WIN32)
    add_definitions(-DFCAppImage)
endif(WIN32)

if(OPENCV2_FOUND)
    add_definitions(-DHAVE_OPENCV2)
endif(OPENCV2_FOUND)

include_directories(
    ${OPENCV2_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${XercesC_INCLUDE_DIRS}
)

set(Image_LIBS
    ${OPENCV2_LIBRARIES}
    FreeCADApp
)

generate_from_xml(ImageObjectPy)
generate_from_xml(ImageColorPy)

set(Python_SRCS
    ImageObjectPy.xml
    ImageObjectPyImp.cpp
    ImageColorPy.xml
    ImageColorPyImp.cpp
)

set(Image_SRCS
    ImageBase.cpp
    ImageBase.h
    ImageCanny.cpp
    ImageCanny.h
    ImageColor.cpp
    ImageColor.h
    ImageContour.cpp
    ImageContour.h
    ImageFilter.cpp
    ImageFilter.h
    ImagePipeline.cpp
    ImagePipeline.h
    ImagePlane.cpp
    ImagePlane.h
    ImageObject.cpp
    ImageObject.h
    ImageThreshold.cpp
    ImageThreshold.h
    PreCompiled.cpp
    PreCompiled.h
    PropertyImage.h
    PropertyImage.cpp
    AppImage.cpp
)

add_library(Image SHARED ${Image_SRCS} ${Python_SRCS})
target_link_libraries(Image ${Image_LIBS})


SET_BIN_DIR(Image Image /Mod/Image)
SET_PYTHON_PREFIX_SUFFIX(Image)

INSTALL(TARGETS Image DESTINATION ${CMAKE_INSTALL_LIBDIR})
