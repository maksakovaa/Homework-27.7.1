# Homework 27.7.1
Добавьте в ваш проект чата логгирование сообщений от других участников чата и своих сообщений.\
Для этого напишите класс Logger, через который будет проходить вся работа с файлом логов. Сохраняйте сообщения в файл на диске, можете назвать его log.txt.
При создании объекта логгера должно производиться открытие файла, а при его разрушении — закрытие файла (концепт RAII).\
Реализуйте в классе два метода:
1) запись строки логов в файл;
2) чтение одной строки из файла.

Конечно же, запись и чтение должны быть потокобезопасными — при одновременном вызове нескольких потоков с записью в файл и чтением из файла, не должно происходить
взаимоблокировок и гонок данных.\
Не забывайте про принципы инкапсуляции: код, пользующийся вашими методами чтения и записи, не должен знать про многопоточность.
# Реализация
- Переписан сетевой класс net как для сервера так и для клиента, реализовано разделение по функциям частей сетевого кода.
- В клиентскую базу сообщений теперь передаются также и "удалённые" пользователи для того чтобы при отображении сообщений удалённого пользователя можно было его идентифицировать по UUID,
 при этом в списке пользователей он не отображается и авторизация под ним не возможна.
- В структуру сообщений добавлен статус сообщений (прочитано/непрочитано)
- Добавлены 2 процедуры и 2 триггера в базу данных для работы со статусом сообщений из общего чата:\
При создании нового сообщения срабатывает триггер который вызывает процедуру добавления строки статуса сообщения в таблицу all_msg_status с UUID всех пользователей
зарегистрированных на данный момент.\
При регистрации нового пользователя срабатывает триггер который вызывает процедуру добавления строки статуса для каждого зарегистрированного сообщения с UUID нового
 пользователя
- В код вывода сообщения на экран добавлен набор инструкций для отметки сообщения как прочитанного. Направляется запрос на сервер и с сервера к базе данных на изменение статуса
- В клиенте добавлено отображение количества непрочитанных сообщений в случае их наличия, как для общего чата так и для приватного.
- Получение базы сообщений происходит при авторизации пользователя, при этом личные сообщения получаются только относящиеся к авторизованному пользователю.
- Добавлена функция-обертка для curDateTime() - logTimeStamp(), для уменьшения текста в коде
- Добавлен класс Logger для сервера. Имеет два метода:\
recLogEntry - для записи в файл. Используется абсолютная блокировка\
readLogEntry - для чтения последней строки из файла. В этом методе используется shared_lock. Также добавлена задержка в 100 микросекунд в связи с тем что при большом количестве запросов
 планировщик выполняет поток раньше чем поток записи в результате читаются старые данные, хотя в лог пишется верно. 
- В классах net и SQL_DB добавлен перехват сообщений для логгера и в новых потоках осуществляется запись в лог + вывод на экран(в консоль). Лог-записи старого образца
(простой вывод в консоль) закомментированы и оставлены для сравнения.
