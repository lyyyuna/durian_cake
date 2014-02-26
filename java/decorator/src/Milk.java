
public class Milk extends CondimentDecorator {
	
	Beverage beverage;
	
	public Milk(Beverage be) {
		this.beverage = be;
	}
	
	public String getDescription() {
		return beverage.getDescription() + ", milk";
	}
	
	public double cost() {
		return 0.1 + beverage.cost();
	}

}
