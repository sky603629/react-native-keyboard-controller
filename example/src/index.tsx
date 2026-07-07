/*
 * Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */

import React, { useCallback, useState } from "react";
import {
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  View,
} from "react-native";
import { KeyboardControllerView } from "react-native-keyboard-controller";

import type { KeyboardControllerProps } from "react-native-keyboard-controller";

type LayoutEvent = Parameters<
  NonNullable<KeyboardControllerProps["onFocusedInputLayoutChanged"]>
>[0];
type SelectionEvent = Parameters<
  NonNullable<KeyboardControllerProps["onFocusedInputSelectionChanged"]>
>[0];
type EventRecord = {
  time: string;
  payload: unknown;
};

const formatTime = () => {
  const date = new Date();
  const hours = String(date.getHours()).padStart(2, "0");
  const minutes = String(date.getMinutes()).padStart(2, "0");
  const seconds = String(date.getSeconds()).padStart(2, "0");
  const milliseconds = String(date.getMilliseconds()).padStart(3, "0");

  return `${hours}:${minutes}:${seconds}.${milliseconds}`;
};

const formatPayload = (record: EventRecord | null) => {
  if (record == null) {
    return "Waiting for event";
  }

  return `${record.time}\n${JSON.stringify(record.payload, null, 2)}`;
};

const INPUTS = [
  "Account number",
  "Customer name",
  "Policy id",
  "Phone",
  "Address",
  "Remark",
];

function App() {
  const [layoutEvent, setLayoutEvent] = useState<EventRecord | null>(null);
  const [selectionEvent, setSelectionEvent] = useState<EventRecord | null>(
    null,
  );

  const handleLayoutChanged = useCallback((event: LayoutEvent) => {
    setLayoutEvent({
      time: formatTime(),
      payload: event.nativeEvent,
    });
  }, []);

  const handleSelectionChanged = useCallback((event: SelectionEvent) => {
    setSelectionEvent({
      time: formatTime(),
      payload: event.nativeEvent,
    });
  }, []);

  return (
    <KeyboardControllerView
      enabled
      onFocusedInputLayoutChanged={handleLayoutChanged}
      onFocusedInputSelectionChanged={handleSelectionChanged}
      style={styles.container}
    >
      <ScrollView
        contentContainerStyle={styles.content}
        keyboardShouldPersistTaps="handled"
      >
        <Text style={styles.title}>KeyboardControllerView events</Text>
        <View style={styles.panel}>
          <Text style={styles.panelTitle}>onFocusedInputLayoutChanged</Text>
          <Text selectable style={styles.eventText}>
            {formatPayload(layoutEvent)}
          </Text>
        </View>
        <View style={styles.panel}>
          <Text style={styles.panelTitle}>onFocusedInputSelectionChanged</Text>
          <Text selectable style={styles.eventText}>
            {formatPayload(selectionEvent)}
          </Text>
        </View>
        <View style={styles.form}>
          {INPUTS.map((placeholder, index) => (
            <TextInput
              key={placeholder}
              multiline={index === INPUTS.length - 1}
              placeholder={placeholder}
              placeholderTextColor="#727272"
              style={[
                styles.input,
                index === INPUTS.length - 1 && styles.multilineInput,
              ]}
              defaultValue={
                index === INPUTS.length - 1
                  ? "Move the caret or select this text to update selection."
                  : undefined
              }
            />
          ))}
        </View>
      </ScrollView>
    </KeyboardControllerView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#F5F7FA",
  },
  content: {
    padding: 20,
    paddingBottom: 56,
  },
  title: {
    color: "#111827",
    fontSize: 22,
    fontWeight: "700",
    marginBottom: 16,
    marginTop: 32,
  },
  panel: {
    backgroundColor: "#FFFFFF",
    borderColor: "#D7DEE8",
    borderRadius: 8,
    borderWidth: 1,
    marginBottom: 12,
    padding: 12,
  },
  panelTitle: {
    color: "#1F2937",
    fontSize: 14,
    fontWeight: "700",
    marginBottom: 8,
  },
  eventText: {
    color: "#243447",
    fontSize: 12,
    lineHeight: 17,
  },
  form: {
    marginTop: 12,
  },
  input: {
    backgroundColor: "#FFFFFF",
    borderColor: "#9AA7B6",
    borderRadius: 8,
    borderWidth: 1,
    color: "#111827",
    fontSize: 16,
    height: 46,
    marginBottom: 14,
    paddingHorizontal: 12,
  },
  multilineInput: {
    height: 92,
    paddingTop: 10,
    textAlignVertical: "top",
  },
});

export default App;
