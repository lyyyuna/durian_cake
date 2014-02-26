
public class PizzaStore {

	SimplePizzaFactory factory;
	
	public PizzaStore(SimplePizzaFactory fact) {
		this.factory = fact;
	}
	
	public Pizza orderPizza(String type) {
		Pizza pizza;
		pizza = factory.createPizza(type);
		
		pizza.prepare();
		pizza.bake();
		pizza.cut();
		pizza.box();
		
		return pizza;
	}
}
