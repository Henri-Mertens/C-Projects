CXX      = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra
TARGET   = optionspricing.exe
BUILDDIR = build

SRCS = main.cpp \
       option.cpp \
       math_utils.cpp \
       pricing/closed_form.cpp \
       pricing/simulation.cpp \
       pricing/greeks.cpp

# Flatten object files into build/ directory
OBJS = $(addprefix $(BUILDDIR)/, $(notdir $(SRCS:.cpp=.o)))

# Tell make to look for .cpp files in . and ./pricing/
vpath %.cpp . pricing

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILDDIR)
	@rm -f $(TARGET)

.PHONY: all clean
