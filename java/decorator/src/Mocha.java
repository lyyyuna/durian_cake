
public class Mocha extends CondimentDecorator {

	Beverage beverage;
	
	public Mocha(Beverage be) {
		this.beverage = be;
	}
	
	public String getDescription() {
		return beverage.getDescription() + ", mocha";
	}
	
	public double cost() {
		return 0.2 + beverage.cost();
	}
}
