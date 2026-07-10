const { Pool } = require('pg');

let pool;

exports.handler = async (event, context) => {
  if (event.httpMethod !== 'GET') {
    return { statusCode: 405, body: 'Method Not Allowed' };
  }

  try {
    if (!pool) {
      pool = new Pool({
        connectionString: process.env.DATABASE_URL,
      });
    }

    // Fetch the 50 most recent interactions, joining with users table to get student names
    const query = `
      SELECT i.id, i.student_id, u.name as student_name, i.button_type, i.created_at 
      FROM iot_interactions i
      LEFT JOIN users u ON i.student_id = u.id
      ORDER BY i.created_at DESC 
      LIMIT 50;
    `;
    
    const result = await pool.query(query);

    return {
      statusCode: 200,
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(result.rows),
    };
  } catch (error) {
    console.error('Error fetching IoT interactions:', error);
    return {
      statusCode: 500,
      body: JSON.stringify({ error: 'Internal Server Error' }),
    };
  }
};
