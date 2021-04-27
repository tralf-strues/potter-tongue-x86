# -----------------------------------Constants----------------------------------
AllWarnings    = -Wall -Wextra -pedantic
SomeWarnings   = -Wall -Wextra
LittleWarnings = -Wall
NoWarnings     = 
# -----------------------------------Constants----------------------------------

Mode = DEBUG_MODE
# ----------------------------------Debug-mode----------------------------------
ifeq ($(Mode), DEBUG_MODE)
	ModeLinkerOptions   = -g
	ModeCompilerOptions = -O1 -g
endif
# ----------------------------------Debug-mode----------------------------------

# ---------------------------------Release-mode---------------------------------
ifeq ($(Mode), RELEASE_MODE)
	ModeLinkerOptions   = 
	ModeCompilerOptions = -O2 -g -DNDEBUG
endif
# ---------------------------------Release-mode---------------------------------

# ------------------------------------Options-----------------------------------
LXXFLAGS = $(ModeLinkerOptions) 
CXXFLAGS = -std=c++20 $(ModeCompilerOptions) $(AllWarnings)
# ------------------------------------Options-----------------------------------

# -------------------------------------Files------------------------------------
SrcDir = src
BinDir = bin
IntDir = $(BinDir)/intermediates
LibDir = libs

CompilerDir = $(SrcDir)/compiler
DynArrayDir = $(SrcDir)/dynamic_array
ParserDir   = $(SrcDir)/parser
SymTableDir = $(SrcDir)/symbol_table

Libs = $(wildcard $(LibDir)/*.a) $(wildcard $(LibDir)/*.h) 

Deps = $(wildcard $(SrcDir)/*.h)      \
	   $(wildcard $(CompilerDir)/*.h) \
	   $(wildcard $(DynArrayDir)/*.h) \
	   $(wildcard $(ParserDir)/*.h)   \
	   $(wildcard $(SymTableDir)/*.h)

CppSrc = $(notdir $(wildcard $(SrcDir)/*.cpp)      \
		          $(wildcard $(CompilerDir)/*.cpp) \
		          $(wildcard $(ParserDir)/*.cpp)   \
		          $(wildcard $(SymTableDir)/*.cpp)) 

Objs = $(addprefix $(IntDir)/, $(CppSrc:.cpp=.o))
Exec = compiler.out
# -------------------------------------Files------------------------------------

# ----------------------------------Make rules----------------------------------
$(BinDir)/$(Exec): $(Objs) $(Deps) $(Libs) 
	$(CXX) -o $(BinDir)/$(Exec) $(Objs) $(Libs) $(LDFLAGS)

vpath %.cpp $(SrcDir) $(CompilerDir) $(DynArrayDir) $(ParserDir) $(SymTableDir)
$(IntDir)/%.o: %.cpp $(Deps)
	$(CXX) -c $< $(CXXFLAGS) -o $@
# ----------------------------------Make rules----------------------------------