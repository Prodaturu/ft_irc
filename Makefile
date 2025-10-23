NAME = IRC
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11
INCDIR = include
OBJDIR = obj

SOURCES = main.cpp \
		  src/Authentication.cpp \
          src/Server.cpp \
          src/Client.cpp \
          src/Channel.cpp \
          src/ChannelModes.cpp \
          src/OperatorCommands.cpp \
		  src/Parser.cpp \
		  src/Commands.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	@$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
