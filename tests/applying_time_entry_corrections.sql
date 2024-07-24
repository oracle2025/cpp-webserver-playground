SELECT *
FROM time_events;
-- Example Result:
-- id,employee_id,event_date,event_time,event_type,corrected_event_id,deleted_event_id,creation_date,creator_user_id

-- Purpose: This script is used to apply time entry corrections to the time_events table.
WITH cteSteps AS (SELECT c1.id         as FinalID,
                         c1.id,
                         c1.employee_id,
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
                         c2.employee_id,
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
-- Example Result:
-- nr,FinalID,id,corrected_event_id,FinalEventType,event_type,FinalDate,FinalTime,lvl
-- 1,00044bc3-c74d-48bb-9e24-748a15968432,00044bc3-c74d-48bb-9e24-748a15968432,"",start,start,2022-09-26,08:00,1

-- Purpose: this script is used to combine the start and stop events into a single row for each day.
SELECT tOn.employee_id, tOn.event_date, tOn.event_time StartTime, tOff.event_date, tOff.event_time EndTime
FROM (WITH cteSteps AS (SELECT c1.id         as FinalID,
                               c1.id,
                               c1.employee_id,
                               c1.corrected_event_id,
                               c1.event_type as FinalEventType,
                               c1.event_type,
                               c1.event_date as FinalDate,
                               c1.event_time as FinalTime,
                               1             as lvl
                        FROM time_events c1
                        WHERE employee_id = 'fceb73d4-630d-41bb-b253-bca924f07108'
                          AND strftime('%Y-%m', event_date) = '2024-07'
                        UNION ALL
                        SELECT cte.FinalID,
                               c2.id,
                               c2.employee_id,
                               c2.corrected_event_id,
                               cte.FinalEventType,
                               c2.event_type,
                               c2.event_date,
                               c2.event_time,
                               cte.lvl + 1
                        FROM cteSteps cte
                                 INNER JOIN time_events c2
                                            ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                        WHERE employee_id = 'fceb73d4-630d-41bb-b253-bca924f07108'
                          AND strftime('%Y-%m', event_date) = '2024-07'),
           cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                           FROM cteSteps)
      SELECT employee_id,
             FinalTime      as event_time,
             FinalDate      as event_date,
             FinalEventType as event_type,
             ROW_NUMBER()      Over (Partition by employee_id order by FinalDate) EventID
      FROM cteNumbered
      WHERE nr = 1
        AND event_type = 'start') tOn
         LEFT JOIN (WITH cteSteps AS (SELECT c1.id         as FinalID,
                                             c1.id,
                                             c1.employee_id,
                                             c1.corrected_event_id,
                                             c1.event_type as FinalEventType,
                                             c1.event_type,
                                             c1.event_date as FinalDate,
                                             c1.event_time as FinalTime,
                                             1             as lvl
                                      FROM time_events c1
                                      WHERE employee_id = 'fceb73d4-630d-41bb-b253-bca924f07108'
                                        AND strftime('%Y-%m', event_date) = '2024-07'
                                      UNION ALL
                                      SELECT cte.FinalID,
                                             c2.id,
                                             c2.employee_id,
                                             c2.corrected_event_id,
                                             cte.FinalEventType,
                                             c2.event_type,
                                             c2.event_date,
                                             c2.event_time,
                                             cte.lvl + 1
                                      FROM cteSteps cte
                                               INNER JOIN time_events c2
                                                          ON c2.corrected_event_id = cte.id OR c2.deleted_event_id = cte.id
                                      WHERE employee_id = 'fceb73d4-630d-41bb-b253-bca924f07108'
                                        AND strftime('%Y-%m', event_date) = '2024-07'),
                         cteNumbered AS (SELECT row_number() over (partition by FinalID order by lvl desc) as nr, *
                                         FROM cteSteps)
                    SELECT employee_id,
                           FinalTime      as event_time,
                           FinalDate      as event_date,
                           FinalEventType as event_type,
                           ROW_NUMBER()      Over (Partition by employee_id order by FinalDate) EventID
                    FROM cteNumbered
                    WHERE nr = 1
                      AND event_type = 'stop') tOff
                   on (tOn.employee_id = tOff.employee_id and tOn.EventID = tOff.EventID);

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


