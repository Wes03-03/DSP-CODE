################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PolarDSP.c \
../src/ad1939.c \
../src/audio_io.c \
../src/distortion.c \
../src/filters.c \
../src/hardware_platform.c 

SRC_OBJS += \
./src/PolarDSP.doj \
./src/ad1939.doj \
./src/audio_io.doj \
./src/distortion.doj \
./src/filters.doj \
./src/hardware_platform.doj 

C_DEPS += \
./src/PolarDSP.d \
./src/ad1939.d \
./src/audio_io.d \
./src/distortion.d \
./src/filters.d \
./src/hardware_platform.d 


# Each subdirectory must supply rules for building sources it contributes
src/PolarDSP.doj: ../src/PolarDSP.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/PolarDSP.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/ad1939.doj: ../src/ad1939.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/ad1939.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/audio_io.doj: ../src/audio_io.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/audio_io.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/distortion.doj: ../src/distortion.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/distortion.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/filters.doj: ../src/filters.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/filters.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/hardware_platform.doj: ../src/hardware_platform.c
	@echo 'Building file: $<'
	@echo 'Invoking: CrossCore SHARC C/C++ Compiler'
	cc21k -c -file-attr ProjectName="PolarDSP" -proc ADSP-21489 -flags-compiler --no_wrap_diagnostics -si-revision 0.2 -g -DCORE0 -D_DEBUG @includes-58d616218a2981aeeda23af81dfdecab.txt -structs-do-not-overlap -no-const-strings -no-multiline -warn-protos -double-size-32 -swc -gnu-style-dependencies -MD -Mo "src/hardware_platform.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


