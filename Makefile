CXX := g++
CXX_FLAGS := -Wall -Wextra -pedantic

SRC_DIR := source
INC_DIR := include
DEPS_DIR := deps
OBJS_DIR := objs

SRCS := $(addprefix $(SRC_DIR)/,main.cpp String.cpp Menu.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
DEPS := $(patsubst $(OBJS_DIR)/%.o,$(DEPS_DIR)/%.d,$(OBJS))
TARGET := program

DBG_FLAGS := -g -D CW_DBG
DBG_DIR := debug
DBG_TARGET := $(DBG_DIR)/$(TARGET)
DBG_OBJS := $(addprefix $(DBG_DIR)/, $(OBJS))

RLS_FLAGS := -O3
RLS_DIR := release
RLS_TARGET := $(RLS_DIR)/$(TARGET)
RLS_OBJS := $(addprefix $(RLS_DIR)/, $(OBJS))

.PHONY: all clean debug release

all: debug release

-include $(addprefix $(DBG_DIR)/,$(DEPS))

debug: $(DBG_TARGET)

$(DBG_TARGET): $(DBG_OBJS)
	$(CXX) $(CXX_FLAGS) $(DBG_FLAGS) -I $(INC_DIR) $^ -o $@

$(DBG_DIR)/$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DBG_DIR)/$(OBJS_DIR) $(DBG_DIR)/$(DEPS_DIR)
	$(CXX) $(CXX_FLAGS) $(DBG_FLAGS) -I $(INC_DIR) -c $< -o $@ \
	-MMD -MP -MF $(patsubst $(DBG_DIR)/$(OBJS_DIR)/%.o,$(DBG_DIR)/$(DEPS_DIR)/%.d,$@)

-include $(addprefix $(RLS_DIR)/,$(DEPS))

release: $(RLS_TARGET)

$(RLS_TARGET): $(RLS_OBJS)
	$(CXX) $(CXX_FLAGS) $(RLS_FLAGS) -I $(INC_DIR) $^ -o $@

$(RLS_DIR)/$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp | $(RLS_DIR)/$(OBJS_DIR) $(RLS_DIR)/$(DEPS_DIR)
	$(CXX) $(CXX_FLAGS) $(RLS_FLAGS) -I $(INC_DIR) -c $< -o $@ \
	-MMD -MP -MF $(patsubst $(RLS_DIR)/$(OBJS_DIR)/%.o,$(RLS_DIR)/$(DEPS_DIR)/%.d,$@)

$(DBG_DIR)/$(OBJS_DIR) $(DBG_DIR)/$(DEPS_DIR) $(RLS_DIR)/$(OBJS_DIR) $(RLS_DIR)/$(DEPS_DIR):
	mkdir -p $@

clean:
	rm -f $(addprefix $(DBG_DIR)/, $(OBJS) $(DEPS) $(TARGET))
	-rmdir $(addprefix $(DBG_DIR)/, $(OBJS_DIR) $(DEPS_DIR)) $(DBG_DIR) --ignore-fail-on-non-empty
	rm -f $(addprefix $(RLS_DIR)/, $(OBJS) $(DEPS) $(TARGET))
	-rmdir $(addprefix $(RLS_DIR)/, $(OBJS_DIR) $(DEPS_DIR)) $(RLS_DIR) --ignore-fail-on-non-empty
