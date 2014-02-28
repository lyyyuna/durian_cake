
public class RemoteControl {

	Command[] onCommand;
	Command[] offCommand; 
	
	public RemoteControl() {
		onCommand = new Command[7];
		offCommand = new Command[7];
		
		Command noCommand = new NoCommand();
		for (int i = 0; i < 7; i++) {
			onCommand[i] = noCommand;
			offCommand[i] = noCommand;
		}
	}
	
	public void setCommand(int slot, Command on, Command off) {
		onCommand[slot] = on;
		offCommand[slot] = off;
	}
	
	public void onButtonWasPressed(int slot) {
		onCommand[slot].execute();
	}
	
	public void offButtonWasPressed(int slot) {
		offCommand[slot].execute();
	}
	
	public String toString() {
		StringBuffer stringBuff = new StringBuffer();
		stringBuff.append("\n------ Remote Control -------\n");
		for (int i = 0; i < onCommand.length; i++) {
			stringBuff.append("[slot " + i + "] " + onCommand[i].getClass().getName()
				+ "    " + offCommand[i].getClass().getName() + "\n");
		}
		return stringBuff.toString();
	}
}
