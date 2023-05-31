
all: parent helper spy sender receiver master_spy child openGl

parent: parent.c
	gcc -g parent.c -o parent

spy: spy.c
	gcc -g spy.c -o spy

helper: helper.c
	gcc -g helper.c -o helper

sender: sender.c
	gcc -g sender.c -o sender
	
receiver: receiver.c
	gcc -g receiver.c -o receiver

masterSpy: master_spy.c
	gcc -g master_spy.c -o master_spy

child: child.c
	gcc -g child.c -o child

openGl: openGl.c
	gcc -g openGl.c -o openGl -lglut -lGLU -lGL -lm

run:
	./parent

clean:
	rm -f parent spy receiver master_spy child sender helper openGl