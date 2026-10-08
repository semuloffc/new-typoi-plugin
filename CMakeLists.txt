cmake_minimum_required(VERSION 3.22)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

project(Petrichor VERSION ${VERSION_STRING})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Petrichor
    COMPANY_NAME "Rflx"
    PLUGIN_MANUFACTURER_CODE Rflx
    PLUGIN_CODE Ptch
    FORMATS VST3 Standalone
    PRODUCT_NAME "Petrichor"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth
)

juce_generate_juce_header(Petrichor)

target_sources(Petrichor PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters/ParameterLayout.cpp
)

target_compile_definitions(Petrichor PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(Petrichor PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
    juce_add_binary_data(PetrichorResources SOURCES
        Resources/Inter-Regular.ttf
        Resources/Inter-Medium.ttf
    )
    target_link_libraries(Petrichor PRIVATE PetrichorResources)
endif()
