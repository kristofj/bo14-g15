public class Hello {
	public static void main(String a[])
	{
		Db empOne = new Db("INSERT INTO logId (datetime) VALUES (Now())");
		System.out.println(empOne.getKey());
	}
}
