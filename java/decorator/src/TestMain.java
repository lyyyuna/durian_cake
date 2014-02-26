
public class TestMain {

	public static void main(String[] args) {
		Beverage be = new Espresso();
		System.out.println(be.getDescription() + " $ " + be.cost());
		
		Beverage be2 = new DarkRoast();
		be2 = new Mocha(be2);
		be2 = new Mocha(be2);
		be2 = new Whip(be2);
		System.out.println(be2.getDescription() + " $ " + be2.cost());
		
	}
}
