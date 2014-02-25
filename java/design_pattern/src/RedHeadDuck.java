
public class RedHeadDuck extends Duck {
	
	public RedHeadDuck() {
		flyBehavior = new FlyWithWings();
		quackBehavior = new Quack();
	}
	
	public void display() {
		System.out.println("i am a real red head duck");
	}

}
