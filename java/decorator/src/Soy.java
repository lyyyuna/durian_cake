
public class Soy extends CondimentDecorator {
	
	Beverage beverage;
	
	public Soy(Beverage be) {
		this.beverage = be;
	}
	
	public String getDescription() {
		return beverage.getDescription() + ", soy";
	}
	
	public double cost() {
		return 0.15 + beverage.cost();
	}

}
