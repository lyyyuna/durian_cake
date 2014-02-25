
public class DecoyDuck extends Duck {
	
	public DecoyDuck() {
		setFlyBehavior(new FlyNoWay());
		setQuackBehavior(new MuteQuack());
	}
	
	public void display() {
		System.out.println("i am a duck decoy");
	}

}
