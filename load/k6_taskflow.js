import http from 'k6/http';
import { check, sleep } from 'k6';

export const options = {
  vus: 11,
  duration: '35s',
  thresholds: {
    http_req_failed: ['rate<0.01'],
    http_req_duration: ['p(95)<380'],
  },
};

export default function () {
  const payload = JSON.stringify({
    title: `ef-${__VU}-${__ITER}`,
    assignee: 'efremov',
    priority: 2,
    dept: 'dev',
  });
  check(http.post('http://localhost:8080/tasks', payload, {
    headers: { 'Content-Type': 'application/json' },
  }), { created: (r) => r.status === 201 });
  check(http.get('http://localhost:8080/tasks'), { listed: (r) => r.status === 200 });
  sleep(0.75);
}
