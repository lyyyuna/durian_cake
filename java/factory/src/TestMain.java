
public class TestMain {

	public static void main(String[] args) {
		SimplePizzaFactory fact = new SimplePizzaFactory();
		PizzaStore store = new PizzaStore(fact);
		
		Pizza pizza = store.orderPizza("cheese");
		System.out.println("we order s " + pizza.getName() + "\n");
	}
}
