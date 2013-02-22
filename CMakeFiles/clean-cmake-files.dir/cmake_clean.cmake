FILE(REMOVE_RECURSE
  "CMakeFiles/clean-cmake-files"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/clean-cmake-files.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
