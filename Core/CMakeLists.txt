add_library(JobSystem
        JobSystem/Job.h
        JobSystem/JobSystem.h
        JobSystem/JobSystem.cpp
)
target_include_directories(JobSystem PUBLIC JobSystem)
target_link_libraries(JobSystem PUBLIC Util)