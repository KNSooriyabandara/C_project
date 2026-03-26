CC = gcc
CFLAGS = -Iinclude -std=c99 -w

# === Task 1: Demo program ===
MAIN_SRC = \
	src/animation.c \
	models/generate_cylinder.c \
	src/lighting.c \
	models/generate_pyramid.c \
	demo/main.c \
	models/generate_soccer_ball.c \
	models/generate_cube.c \
	src/canvas.c \
	src/math3d.c \
	src/renderer.c
MAIN_OUT = build/main.exe

# === Task 2: Math test ===
MATH_SRC = tests/test_math.c src/math3d.c
MATH_OUT = tests/math_tests/test_math.exe

# === Task 3: Rendering pipeline test ===
PIPELINE_SRC = \
	tests/test_pipeline.c \
	src/lighting.c \
	models/generate_cube.c \
	models/generate_cube1.c \
	models/generate_soccer_ball.c \
	src/renderer.c \
	src/canvas.c \
	src/math3d.c
PIPELINE_OUT = tests/visual_tests/test_pipeline.exe

# === Default build target ===
all: $(MAIN_OUT) $(MATH_OUT) $(PIPELINE_OUT)

# === Build Rules ===
$(MAIN_OUT): $(MAIN_SRC)
	@if not exist "build" mkdir "build"
	$(CC) $(CFLAGS) $(MAIN_SRC) -o $(MAIN_OUT)

$(MATH_OUT): $(MATH_SRC)
	@if not exist "tests/math_tests" mkdir "tests/math_tests"
	$(CC) $(CFLAGS) $(MATH_SRC) -o $(MATH_OUT)

$(PIPELINE_OUT): $(PIPELINE_SRC)
	@if not exist "tests/visual_tests" mkdir "tests/visual_tests"
	$(CC) $(CFLAGS) $(PIPELINE_SRC) -o $(PIPELINE_OUT)

# === Run Targets ===
run_main: $(MAIN_OUT)
	./$(MAIN_OUT)

run_math: $(MATH_OUT)
	./$(MATH_OUT)

run_pipeline: $(PIPELINE_OUT)
	@if not exist "tests/visual_tests/visuals" mkdir "tests/visual_tests/visuals"
	@if not exist "tests/visual_tests/visuals/test_cube" mkdir "tests/visual_tests/visuals/test_cube"
	@if not exist "tests/visual_tests/visuals/test_soccer" mkdir "tests/visual_tests/visuals/test_soccer"
	./$(PIPELINE_OUT)

# === GIF Generation for Windows (ImageMagick v7) ===
.PHONY: gif_main gif_soccer all_gifs clean clean_gif

gif_main:
	@echo Creating GIF from main frames...
	@cmd /C "magick -delay 5 -loop 0 fully_rendered_scene\\scene_*.png fully_rendered_scene\\animation.gif"
	@echo Main GIF saved as fully_rendered_scene/animation.gif

gif_soccer:
	@echo Creating GIF from soccer test frames...
	@cmd /C "magick -delay 5 -loop 0 visuals\\test_soccer\\soccerball_*.png visuals\\test_soccer\\test_soccer.gif"
	@echo Soccer GIF saved as visuals/test_soccer/test_soccer.gif

all_gifs: gif_main gif_soccer



# === Cleanup ===
clean:
	@if exist "build" del /Q build\\*.exe
	@if exist "tests/math_tests" del /Q tests\\math_tests\\*.exe
	@if exist "tests/visual_tests" del /Q tests\\visual_tests\\*.exe

clean_gif:
	@if exist "build\\fully_rendered_scene\\animation.gif" del /Q "build\\fully_rendered_scene\\animation.gif"
	@if exist "tests\\visual_tests\\visuals\\test_soccer\\test_soccer.gif" del /Q "tests\\visual_tests\\visuals\\test_soccer\\test_soccer.gif"
