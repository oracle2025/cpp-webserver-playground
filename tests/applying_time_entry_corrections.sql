-- Purpose: This script is used to apply time entry corrections to the time_events table.
WITH cteSteps AS (SELECT c1.id         as FinalID,
                         c1.id,
                         c1.corrected_event_id,
                         c1.event_type as FinalEventType,
                         c1.event_type,
                         c1.event_date as FinalDate,
                         c1.event_time as FinalTime,
                         1             as lvl
                  FROM time_events c1
                  UNION ALL
                  SELECT cte.FinalID,
                         c2.id,
                         c2.corrected_event_id,
                         cte.FinalEventType,
                         c2.event_type,
                         c2.event_date,
                         c2.event_time,
                         cte.lvl + 1
                  FROM cteSteps cte
                           INNER JOIN time_events c2 ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id),
     cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                     FROM cteSteps)
SELECT *
FROM cteNumbered
WHERE nr = 1
  AND FinalEventType != 'correction' AND FinalEventType != 'deletion';

-- Purpose: this script is used to combine the start and stop events into a single row for each day.
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (SELECT employee_id,
             event_time,
             event_date,
             ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
      FROM time_events
      where event_type = 'start'
        AND employee_id = ?
        AND strftime('%Y-%m', event_date) = ?) tOn
         LEFT JOIN (SELECT employee_id,
                           event_time,
                           event_date,
                           ROW_NUMBER() Over (Partition by employee_id order by event_date) EventID
                    FROM time_events
                    where event_type = 'stop'
                      AND employee_id = ?
                      AND strftime('%Y-%m', event_date) = ?) tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);

-- Purpose: this script is used to combine the start and stop events into a single row for each day.
WITH selected_time_events AS (SELECT employee_id,
                                     event_time,
                                     event_date,
                                     ROW_NUMBER() Over (Partition BY employee_id ORDER BY event_date) EventID
                              FROM time_events
                              WHERE event_type = 'start'
                                AND employee_id = ?
                                AND strftime('%Y-%m', event_date) = ?)
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM selected_time_events tOn
         LEFT JOIN selected_time_events tOff
                   ON (tOn.employee_id = tOff.employee_id AND tOn.EventID = tOff.EventID);