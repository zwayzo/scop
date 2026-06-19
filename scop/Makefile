
# ─── Nom du binaire ───────────────────────────────────────────────────────────
NAME		= scop

# ─── Compilateur et flags ─────────────────────────────────────────────────────
CC			= cc
CFLAGS		= -Wall -Wextra -Werror -std=c11 -O2

# ─── Détection de l'OS ────────────────────────────────────────────────────────
OS			:= $(shell uname -s)

ifeq ($(OS), Darwin)
# ── macOS ──
	GLFW_INCLUDES	= -I/opt/homebrew/include
	GLFW_LDFLAGS	= -L/opt/homebrew/lib
	GLFW_LIBS		= -lglfw
	SYS_LIBS		= -framework OpenGL -framework Cocoa \
					  -framework IOKit -framework CoreFoundation
else
# ── Linux (GLFW compilé localement, sans sudo) ──
	GLFW_INSTALL	= libs/glfw_install
	GLFW_INCLUDES	= -I$(GLFW_INSTALL)/include
	GLFW_LDFLAGS	= 
	GLFW_LIBS		= $(GLFW_INSTALL)/lib/libglfw3.a
	SYS_LIBS		= -lGL -lm -ldl -lpthread -lX11 -lXrandr \
					  -lXi -lXxf86vm -lXinerama -lXcursor
endif

# ─── Includes ─────────────────────────────────────────────────────────────────
INCLUDES	= -Iinclude $(GLFW_INCLUDES)

# ─── Sources ──────────────────────────────────────────────────────────────────
SRC_DIR		= src
OBJ_DIR		= obj

SRCS		= $(SRC_DIR)/main.c \
			  $(SRC_DIR)/window.c \
			  $(SRC_DIR)/shader.c \
			  $(SRC_DIR)/mesh.c \
			  $(SRC_DIR)/obj_parser.c \
			  $(SRC_DIR)/texture.c \
			  $(SRC_DIR)/math/vec3.c \
			  $(SRC_DIR)/math/mat4.c \
			  include/glad/glad.c

OBJS		= $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

# ─── Règles principales ───────────────────────────────────────────────────────

all: $(NAME)

$(NAME): $(OBJS)
	@echo "🔗 Linking $(NAME)..."
	@$(CC) $(CFLAGS) $(OBJS) \
		$(GLFW_LDFLAGS) $(GLFW_LIBS) $(SYS_LIBS) \
		-o $(NAME)
	@echo "✅ $(NAME) compilé avec succès !"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "  ⚙️  Compilation $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# ─── Installation des dépendances ─────────────────────────────────────────────

deps:
ifeq ($(OS), Darwin)
	@echo "📦 macOS détecté — installation via Homebrew..."
	@which brew > /dev/null 2>&1 || \
		/bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
	@brew install glfw cmake
	@echo "✅ Dépendances macOS installées !"
else
	@echo "📦 Linux détecté — compilation de GLFW depuis les sources (sans sudo)..."
	@which cmake > /dev/null 2>&1 || \
		(echo "⚠️  cmake introuvable. Tentative via apt..." && \
		(sudo apt-get install -y cmake 2>/dev/null || \
		(mkdir -p $$HOME/.local && \
		wget -q https://github.com/Kitware/CMake/releases/download/v3.28.0/cmake-3.28.0-linux-x86_64.sh \
			-O /tmp/cmake_install.sh && \
		sh /tmp/cmake_install.sh --skip-license --prefix=$$HOME/.local && \
		export PATH=$$HOME/.local/bin:$$PATH)))
	@mkdir -p libs
	@if [ ! -d "libs/glfw" ]; then \
		echo "📥 Clonage de GLFW..."; \
		git clone --depth 1 --branch 3.4.0 \
			https://github.com/glfw/glfw.git libs/glfw; \
	else \
		echo "✅ GLFW déjà cloné, on continue..."; \
	fi
	@echo "🔨 Compilation de GLFW..."
	@cmake -S libs/glfw -B libs/glfw/build \
		-DCMAKE_INSTALL_PREFIX=libs/glfw_install \
		-DBUILD_SHARED_LIBS=OFF \
		-DGLFW_BUILD_EXAMPLES=OFF \
		-DGLFW_BUILD_TESTS=OFF \
		-DGLFW_BUILD_DOCS=OFF \
		-DGLFW_BUILD_WAYLAND=OFF \
		> /dev/null
	@cmake --build libs/glfw/build --parallel > /dev/null
	@cmake --install libs/glfw/build > /dev/null
	@echo "✅ GLFW compilé et installé dans libs/glfw_install !"
endif

# ─── Nettoyage ────────────────────────────────────────────────────────────────

clean:
	@echo "🧹 Suppression des objets..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "🧹 Suppression du binaire..."
	@rm -f $(NAME)

# fclean + suppression des libs compilées (Linux)
distclean: fclean
	@echo "🧹 Suppression des libs locales..."
	@rm -rf libs/glfw/build libs/glfw_install

re: fclean all

# ─── Aide ─────────────────────────────────────────────────────────────────────

help:
	@echo ""
	@echo "  Usage:"
	@echo "    make deps      → installe les dépendances (GLFW)"
	@echo "    make           → compile le projet"
	@echo "    make clean     → supprime les fichiers objets"
	@echo "    make fclean    → supprime les objets + le binaire"
	@echo "    make distclean → fclean + supprime les libs compilées"
	@echo "    make re        → fclean + recompile"
	@echo ""
	@echo "  Lancement:"
	@echo "    ./scop assets/42.obj"
	@echo ""

.PHONY: all clean fclean distclean re deps help