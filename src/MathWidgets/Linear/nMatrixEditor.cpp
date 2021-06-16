#include <mathwidgets.h>

N::MatrixEditor:: MatrixEditor ( QWidget * parent , Plan * p )
                : TableWidget  (           parent ,        p )
                , Object       ( 0 , Types::Matrix           )
                , name         ( ""                          )
                , matrix       ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::MatrixEditor::~MatrixEditor(void)
{
}

void N::MatrixEditor::Configure(void)
{
  verticalHeader   ( ) -> hide ( )                  ;
  horizontalHeader ( ) -> hide ( )                  ;
  ///////////////////////////////////////////////////
  nConnect ( this , SIGNAL(cellChanged (int,int))   ,
             this , SLOT  (valueChanged(int,int)) ) ;
}

void N::MatrixEditor::setHeader(bool visible)
{
  verticalHeader   ( ) -> setVisible ( visible ) ;
  horizontalHeader ( ) -> setVisible ( visible ) ;
}

bool N::MatrixEditor::startup(void)
{
  nKickOut         ( IsNull ( matrix ) , false )                     ;
  blockSignals     ( true                      )                     ;
  setColumnCount   ( matrix -> Columns ( )     )                     ;
  setRowCount      ( matrix -> Rows    ( )     )                     ;
  verticalHeader   ( ) -> hide ( )                                   ;
  horizontalHeader ( ) -> hide ( )                                   ;
  QList<QTableWidgetItem *> Items                                    ;
  for   (int y = 0 ; y < matrix -> Rows    ( ) ; y++ )               {
    for (int x = 0 ; x < matrix -> Columns ( ) ; x++ )               {
      NewTableWidgetItem ( item )                                    ;
      item -> setTextAlignment ( Qt::AlignRight | Qt::AlignVCenter ) ;
      Items << item                                                  ;
      setItem      ( y , x , item              )                     ;
    }                                                                ;
  }                                                                  ;
  blockSignals     ( false                     )                     ;
  Update           (                           )                     ;
  return true                                                        ;
}

void N::MatrixEditor::Update(void)
{
  if (IsNull(matrix)) return                ;
  QList<QTableWidgetItem *> Items           ;
  for (int y=0;y<matrix->Rows();y++)        {
    for (int x=0;x<matrix->Columns();x++)   {
      Items << item ( y , x )               ;
    }                                       ;
  }                                         ;
  ///////////////////////////////////////////
  blockSignals ( true  )                    ;
  switch (matrix->Type())                   {
    case Cpp::Void                          :
    break                                   ;
    case Cpp::Char                          :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        char * d                            ;
        d = (char *)matrix->array()         ;
        char   v = d[i]                     ;
        QString s = QChar(v)                ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::Byte                          :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        uchar * d                           ;
        d = (uchar *)matrix->array()        ;
        uchar   v = d[i]                    ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::Short                         :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        short * d                           ;
        d = (short *)matrix->array()        ;
        short   v = d[i]                    ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::UShort                        :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        ushort * d                          ;
        d = (ushort *)matrix->array()       ;
        ushort   v = d[i]                   ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::Integer                       :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        int * d                             ;
        d = (int *)matrix->array()          ;
        int   v = d[i]                      ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::UInt                          :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        unsigned int * d                    ;
        d = (unsigned int *)matrix->array() ;
        unsigned int   v = d[i]             ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::LongLong                      :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        long long * d                       ;
        d = (long long *)matrix->array()    ;
        long long   v = d[i]                ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::ULongLong                     :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        SUID * d                            ;
        d = (SUID *)matrix->array()         ;
        SUID   v = d[i]                     ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::Float                         :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        float * d                           ;
        d = (float *)matrix->array()        ;
        float  v = d[i]                     ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::Double                        :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        double * d                          ;
        d = (double *)matrix->array()       ;
        double  v = d[i]                    ;
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
    case Cpp::LDouble                       :
      for (int i=0;i<Items.count();i++)     {
        int x = i % matrix->Columns()       ;
        int y = i / matrix->Columns()       ;
        long double * d                     ;
        d = (long double *)matrix->array()  ;
        double  v = (double)d[i]            ; // this is problematic
        QString s = QString::number(v)      ;
        Items[i]->setText ( s )             ;
        setItem ( y , x , Items[i] )        ;
      }                                     ;
    break                                   ;
  }                                         ;
  for (int i=0;i<matrix->Columns();i++)     {
    resizeColumnToContents ( i )            ;
  }                                         ;
  blockSignals ( false )                    ;
}

void N::MatrixEditor::valueChanged(int row,int column)
{
  if (IsNull(matrix)) return                  ;
  blockSignals ( true  )                      ;
  QTableWidgetItem * Item                     ;
  int id = -1                                 ;
  id   = row * matrix->Columns()              ;
  id  += column                               ;
  Item = item ( row , column )                ;
  switch (matrix->Type())                     {
    case Cpp::Void                            :
    break                                     ;
    case Cpp::Char                            :
      if (NotNull(Item))                      {
        char * d                              ;
        d = (char *)matrix->array()           ;
        char   v                              ;
        QString V = Item->text()              ;
        if (V.length()>0)                     {
          QChar C = V.at(0)                   ;
          v = C.toLatin1()                    ;
          d[id] = v                           ;
        }                                     ;
      }                                       ;
    break                                     ;
    case Cpp::Byte                            :
      if (NotNull(Item))                      {
        uchar * d                             ;
        d = (uchar *)matrix->array()          ;
        uchar   v                             ;
        QString V = Item->text()              ;
        v = (uchar)V.toInt ()                 ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::Short                           :
      if (NotNull(Item))                      {
        short * d                             ;
        d = (short *)matrix->array()          ;
        short   v                             ;
        QString V = Item->text()              ;
        v = V.toShort ()                      ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::UShort                          :
      if (NotNull(Item))                      {
        ushort * d                            ;
        d = (ushort *)matrix->array()         ;
        ushort   v                            ;
        QString V = Item->text()              ;
        v = V.toUShort ()                     ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::Integer                         :
      if (NotNull(Item))                      {
        int * d                               ;
        d = (int *)matrix->array()            ;
        int   v                               ;
        QString V = Item->text()              ;
        v = V.toInt ()                        ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::UInt                            :
      if (NotNull(Item))                      {
        unsigned int * d                      ;
        d = (unsigned int *)matrix->array()   ;
        unsigned int   v                      ;
        QString V = Item->text()              ;
        v = V.toUInt ()                       ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::LongLong                        :
      if (NotNull(Item))                      {
        long long * d                         ;
        d = (long long *)matrix->array()      ;
        long long   v                         ;
        QString V = Item->text()              ;
        v = V.toLongLong ()                   ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::ULongLong                       :
      if (NotNull(Item))                      {
        SUID * d                              ;
        d = (SUID *)matrix->array()           ;
        SUID  v                               ;
        QString V = Item->text()              ;
        v = V.toULongLong ()                  ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::Float                           :
      if (NotNull(Item))                      {
        float * d                             ;
        d = (float *)matrix->array()          ;
        float  v                              ;
        QString V = Item->text()              ;
        v = V.toFloat()                       ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::Double                          :
      if (NotNull(Item))                      {
        double * d                            ;
        d = (double *)matrix->array()         ;
        double  v                             ;
        QString V = Item->text()              ;
        v = V.toDouble()                      ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
    case Cpp::LDouble                         :
      if (NotNull(Item))                      {
        long double * d                       ;
        d = (long double *)matrix->array()    ;
        long double   v                       ;
        QString V = Item->text()              ;
        v = V.toDouble()                      ;
        d[id] = v                             ;
      }                                       ;
    break                                     ;
  }                                           ;
  blockSignals ( false  )                     ;
  Alert        ( Action )                     ;
}
