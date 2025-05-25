

CXXFLAGS = -I/usr/include/opencv4
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc

main: main.cpp Game.cpp Game.h Branch.cpp Player.cpp Player.h Tree.cpp Branch.h Tree.h WateringAction.cpp FertilisingAction.cpp PruningAction.cpp Timeline.h WateringAction.h FertilisingAction.h GrowingAction.cpp GrowingAction.h Printable.h PruningAction.h Timeline.h Clickable.h Timeline.cpp
	g++ main.cpp Game.cpp Branch.cpp Player.cpp Tree.cpp WateringAction.cpp FertilisingAction.cpp PruningAction.cpp GrowingAction.cpp Timeline.cpp -o main $(CXXFLAGS) $(LDFLAGS)

test: test.cpp Game.cpp Game.h Branch.cpp Player.cpp Player.h Tree.cpp Branch.h Tree.h WateringAction.cpp FertilisingAction.cpp  PruningAction.cpp Timeline.h WateringAction.h FertilisingAction.h GrowingAction.cpp GrowingAction.h Printable.h PruningAction.h Timeline.h Clickable.h
	g++ test.cpp Game.cpp Branch.cpp Player.cpp Tree.cpp WateringAction.cpp FertilisingAction.cpp PruningAction.cpp GrowingAction.cpp Timeline.cpp -o Test $(CXXFLAGS) $(LDFLAGS)
	./Test

clean:
	rm -f *.o main Test savegame.txt