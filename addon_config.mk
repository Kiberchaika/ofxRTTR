meta:
	ADDON_NAME = ofxRTTR

common:
	ADDON_INCLUDES_EXCLUDE += libs/librttr/include/rttr/%
	ADDON_INCLUDES_EXCLUDE += libs/librapidjson/include/rapidjson/%

	ADDON_SOURCES_EXCLUDE += libs/librttr/include/rttr/%
	ADDON_SOURCES_EXCLUDE += libs/librapidjson/include/rapidjson/%

vs:
	# for dynamic dll
	# ADDON_DEFINES += RTTR_DLL;%(PreprocessorDefinitions)
