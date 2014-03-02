NAME	=	solver

CXX	=	clang++

SRC	=	main.cpp \
		z3ann.cpp \
		BackendC.cpp

OBJ	=	$(SRC:.cpp=.o)

CXXFLAGS	=	-Wall -Wextra -O3

LDFLAGS	=	-lz3

$(NAME):	$(OBJ)
		$(CXX) $(OBJ) $(LDFLAGS) -o $(NAME)

clean:
		rm -rf $(OBJ)

fclean:		clean
		rm -rf $(NAME)

re:		fclean $(NAME)

ex:		$(NAME)
		./$(NAME)

cex:		re
		./$(NAME)

%.o:%.cpp	$(CXX) $(CXXFLAGS) -c $< -o $@