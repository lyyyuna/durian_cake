
public class GarageDoorOffCommand implements Command {

	GarageDoor garageDoor;
	
	public GarageDoorOffCommand(GarageDoor garageDoor) {
		this.garageDoor = garageDoor;
	}
	
	public void execute() {
		garageDoor.down();
	}
}
