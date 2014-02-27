
public class CheesePizza extends Pizza {
	
	PizzaIngredientFactory ingredientFactory;
	
	public CheesePizza(PizzaIngredientFactory infactory) {
		this.ingredientFactory = infactory;
	}
	
	void prepare() {
		System.out.println("preparing " + name);
		dough = ingredientFactory.createDough();
		sauce = ingredientFactory.createSauce();
		cheese = ingredientFactory.createCheese();
	}

}
