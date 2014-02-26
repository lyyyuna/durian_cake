
public class Whip extends CondimentDecorator {
	
	Beverage beverage;
	
	public Whip(Beverage be) {
		this.beverage = be;
	}
	
	public String getDescription() {
		return beverage.getDescription() + ", whip";
	}
	
	public double cost() {
		return 0.111 + beverage.cost();
	}

}
