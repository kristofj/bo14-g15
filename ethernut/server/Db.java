import java.sql.*;

//håndterer tilkoblinger til databasen
public class Db {
    static final String DB_URL = "jdbc:mysql://host/database_name";
    static final String USER = "username";
    static final String PASS = "password";
    int key = 0;

    //konstruktøren tar imot SQL-setningen som skal kjøres opp mot databasen
    public Db(String sql) {
        Connection conn = null;
        Statement stmt = null;
        try {
            Class.forName("com.mysql.jdbc.Driver");
            conn = DriverManager.getConnection(DB_URL, USER, PASS);
            stmt = conn.prepareStatement(sql);
            stmt.executeUpdate(sql, Statement.RETURN_GENERATED_KEYS);
            ResultSet rs = stmt.getGeneratedKeys();
            rs = stmt.getGeneratedKeys();
            if (rs.next()) {
                key = rs.getInt(1);
            } else {
            }
            rs.close();
            stmt.close();
            conn.close();
            System.out.println("Executed: " + sql);
        }
        //catcher exceptions og printer de ut. (Merk: stopper ikke opp programmet.
        // Dette er et bevisst valg, da det sikrer at andre ikke kan stoppe programmet
        // ved å sende malformerte pakker)
        catch (SQLException se) {
            System.out.println(se.toString());
        } catch (Exception e) {
            System.out.println(e.toString());
        } finally {
            try {
                if (stmt != null)
                    stmt.close();
            } catch (SQLException se2) {
                System.out.println(se2.toString());
            }
            try {
                if (conn != null)
                    conn.close();
            } catch (SQLException se) {
                System.out.println(se.toString());
            }
        }
    }
    //returnerer nøkkelen generert av MySQL
    public int getKey() {































        return key;
    }
}
