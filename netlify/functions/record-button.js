const { Pool } = require('pg');

// Use outside connection to reuse it across function invocations
let pool;

exports.handler = async (event, context) => {
  // Only allow POST requests
  if (event.httpMethod !== 'POST') {
    return { statusCode: 405, body: 'Method Not Allowed' };
  }

  try {
    if (!pool) {
      pool = new Pool({
        connectionString: process.env.DATABASE_URL,
      });
    }

    // Parse the incoming JSON from ESP32
    const body = JSON.parse(event.body);
    const { student_id, button_type } = body;

    if (!student_id || !button_type) {
      return { 
        statusCode: 400, 
        body: JSON.stringify({ error: 'Missing student_id or button_type' }) 
      };
    }

    console.log(`Received interaction: Student ${student_id} pressed ${button_type}`);

    // Insert into Neon DB
    const query = `
      INSERT INTO iot_interactions (student_id, button_type) 
      VALUES ($1, $2) RETURNING *;
    `;
    const result = await pool.query(query, [student_id, button_type]);

    return {
      statusCode: 200,
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        message: 'Interaction recorded successfully',
        data: result.rows[0]
      }),
    };
  } catch (error) {
    console.error('Error inserting IoT interaction:', error);
    return {
      statusCode: 500,
      body: JSON.stringify({ error: 'Internal Server Error' }),
    };
  }
};
