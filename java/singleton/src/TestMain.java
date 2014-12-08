
public class TestMain {

	public static void main(String[] args) {
		Singleton foo = CoolerSingleton.getInstance();
		Singleton bar = HotterSingleton.getInstance();
		System.out.println(foo);
		System.out.println(bar);
		
		System.out.println();
		System.out.println();
		System.out.println();
	}
}
