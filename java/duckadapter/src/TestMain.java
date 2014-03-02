
public class TestMain {

	public static void main(String[] args) {
		MallardDuck duck = new MallardDuck();
		
		WildTurkey turkey = new WildTurkey();
		Duck turkeyAdapter = new TurkeyAdapter(turkey);
		
		System.out.println("the turkey says ...");
		turkey.gobble();
		turkey.fly();
		
		System.out.println("the duck says ...");
		testDuck(duck);
		
		System.out.println("the turkeyadapter says");
		testDuck(turkeyAdapter);
	}
	
	static void testDuck(Duck duck) {
		duck.quack();
		duck.fly();
	}
}
