CXX=g++

LIBS=-lGLU -lglut -IGLU

tankX: tankX.cpp mat4.o vec4.o mesh.o game_object.o
	${CXX} -o tankX tankX.cpp mat4.o vec4.o mesh.o game_object.o ${LIBS}

mat4.o: mat4.cpp mat4.h
	${CXX} -c mat4.cpp mat4.h ${LIBS}

vec4.o: vec4.cpp vec4.h mat4.h
	${CXX} -c vec4.cpp vec4.h ${LIBS}

mesh.o: mesh.cpp mesh.h mat4.h vec4.h
	${CXX} -c mesh.cpp mesh.h ${LIBS}

game_object.o: game_object.cpp game_object.h vec4.h mesh.h
	${CXX} -c game_object.cpp game_object.h ${LIBS}

clean:
	rm -f *.o *.gch tankX
