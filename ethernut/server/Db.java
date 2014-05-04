import java.sql.*;

public class Db {
    static final String DB_URL = "jdbc:mysql://j3nsen.com/hessdalen_weather";
    static final String USER = "hessdalen";
    static final String PASS = "hessdalen123";
    int key = 0;

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
        } catch (SQLException se) {
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

    public int getKey() {































        return key;
    }
}
