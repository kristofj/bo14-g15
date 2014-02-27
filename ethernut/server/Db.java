import java.sql.*;
import java.util.Properties;

public class Db {
	String sql;
	static final String JDBC_DRIVER = "com.mysql.jdbc.Driver";  
	static final String DB_URL = "jdbc:mysql://j3nsen.com/hessdalen_weather";  
	static final String USER = "hessdalen";
	static final String PASS = "hessdalen123";
	int key=0;
   
	public Db(String sql) {
		Connection conn = null;
		Statement stmt = null;
		try{
			//STEP 2: Register JDBC driver
			Class.forName("com.mysql.jdbc.Driver");

			//STEP 3: Open a connection
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection(DB_URL,USER,PASS);

			//STEP 4: Execute a query
			stmt = conn.prepareStatement(sql);
			stmt.executeUpdate(sql, Statement.RETURN_GENERATED_KEYS);
			ResultSet rs = stmt.getGeneratedKeys();	
			int autoIncKeyFromApi = -1;
			rs = stmt.getGeneratedKeys();
			if (rs.next()) {
				key = rs.getInt(1);
				System.out.println(key);
			} else {
				// throw an exception from here
			}
			//STEP 6: Clean-up environment
			rs.close();
			stmt.close();
			conn.close();
			}catch(SQLException se){
			se.printStackTrace();
			}catch(Exception e){
			e.printStackTrace();
			}finally{
			try{
				if(stmt!=null)
				stmt.close();
			}catch(SQLException se2){
			}
			try{
				if(conn!=null)
				conn.close();
			}catch(SQLException se){
				se.printStackTrace();
			}
		}
	}
	public int getKey(){
		return key;
	}
}
